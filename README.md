This is a library with many calendrical calculations. **It is abandoned.** I no longer plan to develop the library further.

You are free to use this code under the terms of the LICENSE.md but you will have to maintain it yourself.

## Post-mortem

The library requires zig 0.9.1, and does not compile with zig 0.10.x.

I will probably need to rewrite large portions of the library, not only due to the incompatibility with zig 0.10.x but also due to changing goals of the library. One aim of this library was to create a data structure that could represent any calendar system, but I no longer believe this goal is practical or even desirable.

However large portions of the code, including the cross-language API, was created with this ill-considered aim in mind. Even if I were to maintain this library, a breaking change would be inevitable.
