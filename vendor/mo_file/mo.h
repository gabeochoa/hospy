/*

The MIT License

Copyright (c) 2007 Jonathan Blow (jon [at] number-none [dot] com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

/*
 This code is designed to be a very simple drop-in method for reading .mo
 files, which are a standard format for storing text strings to help with
 internationalization.

 For more information, see:
 http://www.gnu.org/software/gettext/manual/gettext.html

 Unfortunately the gettext code is unwieldy.  For a simple program like
 a video game, we just want to look up some strings in a pre-baked file,
 which is what this code does.

 We assume that you started with a .po file (which is a human-readable format
 containing all the strings) and then compiled it into a .mo file (which is
 a binary format that can be efficiently read into memory all in one chunk).
 This code then reads straight from the .mo file, so there is no extra
 string allocation and corresponding memory fragmentation.

 You can generate a .mo file from a .po file using programs such as poedit
 or msgfmt.

 This code assumes you compiled the hash table into the .mo file.  It also
 doesn't attempt to care about the encoding of your text; it assumes you
 already know what that is.  (I use UTF-8 which seems like the only sane
 choice).

 There's no good reason that this is a C++ file, rather than a C file; I just
 wrote it that way originally.  You can trivially get rid of the C++-ness if
 you want it to compile as straight C.

 Send questions to jon [at] number-none [dot] com.

 21 December 2007
*/

#pragma once

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

namespace i18n {

struct LocalizationText {
    LocalizationText(const char *filename);
    ~LocalizationText();

    void abort();

    void *mo_data;
    int reversed;

    int num_strings;
    int original_table_offset;
    int translated_table_offset;
    int hash_num_entries;
    int hash_offset;
};

// This is just the common hashpjw routine, pasted in:

#define HASHWORDBITS 32

inline unsigned long hashpjw(const char *str_param) {
    unsigned long hval = 0;
    unsigned long g;
    const char *s = str_param;

    while (*s) {
        hval <<= 4;
        hval += (unsigned char) *s++;
        g = hval & ((unsigned long int) 0xf << (HASHWORDBITS - 4));
        if (g != 0) {
            hval ^= g >> (HASHWORDBITS - 8);
            hval ^= g;
        }
    }

    return hval;
}

// Read an entire file into memory.  Replace this with any equivalent function.

#ifdef WIN32
#define fileno _fileno
#define fstat _fstat
#define stat _stat
#endif
inline int os_read_entire_file(FILE *file, void **data_return) {
    assert(file);
    int descriptor = fileno(file);

    struct stat file_stats;
    int result = fstat(descriptor, &file_stats);
    if (result == -1) return -1;

    int length = (int) file_stats.st_size;

    unsigned char *data = new unsigned char[length];

    fseek(file, 0, SEEK_SET);
    int success = (int) fread((void *) data, length, 1, file);
    if (success < 1) {
        delete[] data;
        return -1;
    }

    *data_return = data;
    return length;
}

// Swap the endianness of a 4-byte word.
// On some architectures you can replace my_swap4 with an inlined instruction.
inline unsigned long my_swap4(unsigned long result) {
    unsigned long c0 = (result >> 0) & 0xff;
    unsigned long c1 = (result >> 8) & 0xff;
    unsigned long c2 = (result >> 16) & 0xff;
    unsigned long c3 = (result >> 24) & 0xff;

    return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
}

inline int read4_from_offset(LocalizationText *loc, int offset) {
    unsigned long *ptr = (unsigned long *) (((char *) loc->mo_data) + offset);

    if (loc->reversed) {
        return (int) my_swap4(*ptr);
    } else {
        return (int) *ptr;
    }
}

inline char *get_source_string(LocalizationText *loc, int index) {
    int addr_offset = loc->original_table_offset + 8 * index + 4;
    int string_offset = read4_from_offset(loc, addr_offset);

    char *t = ((char *) loc->mo_data) + string_offset;
    return t;
}

inline char *get_translated_string(LocalizationText *loc, int index) {
    int addr_offset = loc->translated_table_offset + 8 * index + 4;
    int string_offset = read4_from_offset(loc, addr_offset);

    char *t = ((char *) loc->mo_data) + string_offset;
    return t;
}

static bool label_matches(LocalizationText *loc, const char *s, int index) {
    char *t = get_source_string(loc, index);
    if (strcmp(s, t) == 0) return true;

    return false;
}

inline int get_target_index(LocalizationText *loc, const char *s) {
    unsigned long V = hashpjw(s);
    int S = loc->hash_num_entries;

    int hash_cursor = V % S;
    int orig_hash_cursor = hash_cursor;
    int increment = 1 + (V % (S - 2));

    while (1) {
        unsigned int index =
            read4_from_offset(loc, loc->hash_offset + 4 * hash_cursor);
        if (index == 0) break;

        index--;  // Because entries in the table are stored +1 so that 0 means
                  // empty.

        if (label_matches(loc, s, index)) return index;

        hash_cursor += increment;
        hash_cursor %= S;

        if (hash_cursor == orig_hash_cursor) break;
    }

    return -1;
}

inline LocalizationText::LocalizationText(const char *filename) {
    mo_data = NULL;
    num_strings = 0;
    original_table_offset = 0;
    translated_table_offset = 0;
    hash_num_entries = 0;
    hash_offset = 0;

    FILE *f = fopen(filename, "rb");
    if (!f) return;

    void *data;
    int length = os_read_entire_file(
        f, &data);  // Replace this with any function that will read an entire
                    // file and return it in a block of newly-allocated memory.
    fclose(f);

    if (length < 0) return;  // os_read_entire_file returns -1 on failure.
    if (length < 24) {  // There has to be at least this much in the header...
        abort();
        return;
    }

    mo_data = data;

    unsigned long *long_ptr = (unsigned long *) data;

    const unsigned long TARGET_MAGIC = 0x950412DE;
    const unsigned long TARGET_MAGIC_REVERSED = 0xDE120495;
    unsigned long magic = long_ptr[0];

    if (magic == TARGET_MAGIC) {
        reversed = 0;
    } else if (magic == TARGET_MAGIC_REVERSED) {
        reversed = 1;
    } else {
        abort();
        return;
    }

    num_strings = read4_from_offset(this, 8);
    original_table_offset = read4_from_offset(this, 12);
    translated_table_offset = read4_from_offset(this, 16);
    hash_num_entries = read4_from_offset(this, 20);
    hash_offset = read4_from_offset(this, 24);

    if (hash_num_entries ==
        0) {  // We expect a hash table to be there; if it's not, bail.
        abort();
        return;
    }
}

inline void LocalizationText::abort() {
    delete[] (char *) mo_data;
    mo_data = NULL;
    return;
}

inline LocalizationText::~LocalizationText() {
    if (mo_data) delete[] ((char *) mo_data);
}

//
// Call text_lookup to go from a source string to a translated string.
// This is the main entry point that you would use.  (The only other
// entry point is to construct a Localization_Text() to initialize
// things.)
//

/*
Localization_Text *the_localization_text;
char *text_lookup(char *s) {
    Localization_Text *loc = the_localization_text;
    if (!loc->mo_data) return s;

    int target_index = get_target_index(loc, s);
    if (target_index == -1) return s;  // Maybe we want to log an error?

    return get_translated_string(loc, target_index);
}
int main() {
the_localization_text = new Localization_Text();

printf("The string for 'hello' is: %s\n", text_lookup("hello"));
}

*/

}  // namespace i18n
