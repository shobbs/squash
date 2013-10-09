@section intro Introduction

Squash is an abstraction layer which provides a single API to access
many compression libraries.

For a high-level overview of how to use Squash, please see the
[Squash User Guide](@ref md_user-guide).

@subsection bindings Bindings

Squash is written in C and has minimal hard dependencies, meaning it
should be straightforward to create bindings for many languages.  In
order help with testing, bindings are generally distributed with
Squash.

Squash currently supports the following languages:

- C
- Vala

We hope to add more soon.  If you're interested in helping, please
contact us!

@subsection plugins Plugins

The actual integration with individual compression libraries is done
through plugins which can be installed separately from Squash itself
and are not loaded until they are required.  This allows Squash
consumers to utilize a great many compression algorithms without
rewriting code or unnecessary bloat.  Plugins need not be distributed
with Squash, though it is preferred.

Squash currently contains plugins for the following libraries:

- [Blosc](@ref md_blosc)
- [bzip2](@ref md_bzip2)
- [Doboz](@ref md_doboz)
- [FastLZ](@ref md_fastlz)
- [LZ4](@ref md_lz4)
- [liblzma](@ref md_lzma)
- [LZMAT](@ref md_lzmat)
- [LZF](@ref md_lzf)
- [liblzg](@ref md_lzg)
- [LZO](@ref md_lzo)
- [QuickLZ](@ref md_quicklz)
- [SHARC](@ref md_sharc)
- [Snappy](@ref md_snappy)
- [wfLZ](@ref md_wflz)
- [zlib](@ref md_zlib)
- [ZPAQ](@ref md_zpaq)

We hope to add more soon.  If you're interested in helping, please
contact us!

@subsection license License

Squash is licensed under the [MIT
license](http://opensource.org/licenses/MIT).  Although this license
does not place many restrictions on using Squash itself, please keep
in mind that some plugins use libraries which are subject to more
restrictive terms (such as
[LZO](http://www.oberhumer.com/opensource/lzo/)).