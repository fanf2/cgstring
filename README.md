Experimenting with `_Generic()` for parametric constness
========================================================

One of the new features in C99 was `tgmath.h`, the type-generic
mathematics libraty. (See section 7.22 of
[n1256](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf).)
This added ad-hoc polymorphism for a subset of the library, but C99
offered no way for programmers to write their own type-generic macros
in standard C.

In C11 the language acquired the `_Generic()` generic selection
operator. (See section 6.5.1.1 of
[n1570](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf).)
This is effectively a type-directed switch expression. It can be used
to implement APIs like `tgmath.h` using standard C.

(The weird spelling with an underscore and capital letter is because
that part of the namespace is reserved for future language extensions.)


When `const` is ugly
--------------------

It can often be tricky to write const-correct code in C, and
retrofitting constness to an existing API is much worse.

There are some fearsome examples in the standard library. For
instance, `strchr()` is declared:

        char *strchr(const char *s, int c);

(See section 7.24.5.2 of
[n1570](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf).)

That is, it takes a const string as an argument, indicating that it
doesn't modify the string, but it returns a non-const pointer into the
same string. This hidden de-constifying cast allows `strchr()` to be
used with non-const strings as smoothly as with const strings, since
in either case the implicit type conversions are allowed. But it is an
ugly loop-hole in the type system.


Parametric constness
--------------------

It would be much better if we could write something like,

        const<A> char *strchr(const<A> char *s, int c);

where `const<A>` indicates variable constness. Because the same
variable appears in the argument and return types, those strings are
either both const or both mutable.

When checking the function definition, the compiler would have to
treat parametric `const<A>` as equivalent to a normal const qualifier.
When checking a call, the compiler allows the argument and return
types to be const or non-const, provided they match where the
parametric consts indicate they should.

But we can't do that in standard C.


Or can we?
----------

When I mentioned this idea on Twitter a few days ago,
[Joe Groff said "`_Generic` to the
rescue"](https://twitter.com/jckarter/status/736687735038054400),
so I had to see if I could make it work.


Example: `strchr()`
-------------------

Before wrapping a standard function with a macro, we have to remove
any existing wrapper. (Standard library functions can be wrapped by
default!)

        #ifdef strchr
        #undef strchr
        #endif

Then we can create a replacement macro which implements parametric
constness using `_Generic()`.

        #define strchr(s,c) _Generic((s),                    \
            const char * : (const char *)(strchr)((s), (c)), \
            char *       :               (strchr)((s), (c)))

The first line says, look at the type of the argument `s`.

The second line says, if it is a `const char *`, call the function
`strchr` and use a cast to restore the missing constness.

The third line says, if it is a plain `char *`, call the function
`strchr` leaving its return type unchanged from `char *`.

The `(strchr)()` form of call is to avoid warnings about attempts to
invoke a macro recursively.

        void example(void) {
            const char *msg = "hello, world\n";
            char buf[20];
            strcpy(buf, msg);

            strchr(buf, ' ')[0] = '\0';
            strchr(msg, ' ')[0] = '\0';
            strchr(10,20);
        }

In this example, the first call to `strchr` is always OK.

The second call typically fails at runtime with the standard `strchr`,
but with parametric constness you get a compile time error saying that
you can't modify a const string.

Without parametric constness the third call gives you a type
conversion warning, but it still compiles! With parametric constness
you get an error that there is no matching type in the `_Generic()`
macro.


Conclusion
----------

That is actually pretty straightforward, which is nice.

As well as parametric constness for functions, in the past I have also
wondered about parametric constness for types, especially structures.
It would be nice to be able to use the same code for read-only static
data as well as mutable dynamic data, and have the compiler enforce
the distinction. But `_Generic()` isn't powerful enough, and in any
case I am not sure how such a feature should work!
