This is a toolkit for creating classic text adventures. Its core is a library written
with modern C++ which defines a representation for verbs, objects and actions and
which parses a text adventure DSL. It also contains CLI programs both for translating
between the binary representation and the DSL as well as for playing these adventures.

Finally, the repository also contains an experimental web player using
WebAssembly/Emscripten and some hand-written HTML/CSS/Javascript.

It allows you to write text adventures in an efficient-to-write, easy-to-read language,
have them translated to a binary and publish them as browser games.

See the `example.ta` file for an example.

# To-Do
* Reduce code size, especially that of the WebAssembly.
* Improve comment and whitespace handling.
* Improve compiling on Windows.

# How to use
1. Compile the CLI programs using `./compilecli.sh g++` (or `./compilecli.sh clang++` respectively).
2. Compile your text adventure using `TextLangCompiler.build [YOUR FILE] test`.
3. Use `TextLangPlayer.build` to try it out.
4. Compile the web player using `./compilejs.sh` and store your `test` file (created above)
   in the `Build/html` directory.
5. Now publish your `Build/html` directory and point a browser to it.
