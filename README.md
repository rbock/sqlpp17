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

Note:
-----
If you are looking for a production-ready embedded domain specific language for SQL in C++, use sqlpp11.

License:
-------------
sqlpp17 is published under the [BSD 2-Clause License](https://github.com/rbock/sqlpp17/blob/master/LICENSE).

