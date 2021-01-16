sqlpp17
=======
This is an experimental rewrite of [sqlpp11](https://github.com/rbock/sqlpp11) to C++17.

It is done from scratch. C++17 makes a lot of things much simpler. Some aspects I described in my CppCon 2017 talk:

https://www.youtube.com/watch?reload=9&v=bA7b1HEFVFk

However, C++17 is also lacking a couple of really important things, e.g.

  - constexpr containers (C++20 will have string and vector)
  - constexpr typeid (C++20)
  - concepts (C++20)
  - reflection (C++??)

Most of the required functionality is present in sqlpp17, but it is not well tested. And seeing what can be expected from C++20, I currently don't believe that I will finish sqlpp17, but rather advance to C++20, soon.

Usage:
-----

**Use with cmake**: The library officially supports two ways how it can be used with cmake. You can find examples for both methods in the example folder.

1. Fetch content (**Recommended**, no installation required): See `examples/<connector>/usage_fetchContent`
1. Find package (installation required, see above): See `examples/<connector>/usage_findPackage`

Build and Install:
-----

**Build from Source:**

Download and unpack the latest release from https://github.com/rbock/sqlpp17/releases or clone the repository. Inside the directory run the following commands:

```cmake
cmake -B build
cmake --build build --target install
```
**Note**: You might need admin rights for the last comman

Note:
-----
If you are looking for a production-ready embedded domain specific language for SQL in C++, use sqlpp11.

License:
-----
sqlpp17 is published under the [BSD 2-Clause License](https://github.com/rbock/sqlpp17/blob/master/LICENSE).

