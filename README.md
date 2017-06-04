# tmp-lib
Template magic library

## Random tricks

You may find these useful when you have no idea how to do *that* template magic. 

* Union. Nowadays unions have no other use case than adding workaround possibilities to the standard. Hint: a reinterpret_cast is forbidden at compile time via unions as well, but members' address may be taken. Example: [offset_of](https://github.com/cpp-ftw/tmp-lib/blob/master/src/Property.hpp).

* Fake a method by a member with a function call operator. Example: [Property](https://github.com/cpp-ftw/tmp-lib/blob/master/src/Property.hpp)

* You can enumerate an argument pack's types by their indices as well. Example: [variadic_insert](https://github.com/cpp-ftw/tmp-lib/blob/master/src/variadic_insert.hpp)

* Each lambda has a unique type. If you have two template stuff that should generate different template instances, extend it with a dummy lambda template parameter. Example: [to_fptr](https://github.com/cpp-ftw/tmp-lib/blob/master/src/to_fptr.hpp).

* Have a lambda (member) instead of a regular function, as they have special skills. They might be stored in variables even if templated. Example: [overload](https://pastebin.com/3tvVxwsz)

* If you are struggling with passing stuff at compile time, create a carrier struct with regular members initialized at the declaration, and pass that as a typename. You may default construct them anywhere and their members will instantly contain your value. You can hide the additional boilerplate with a macro. Example: [overload](https://pastebin.com/3tvVxwsz)

* Everything fits into a `void*` in a type safe manner: functions, methods, arguments, etc. Create an aggregate struct with any data, and the first member should be a function pointer that knows everything about the latter members, and unpacks the original `void*`. When you get that `void* data`, calling `(*static_cast<unpacker*>(data))(data)` will do the job.

* Create a dummy function with no definitions to substitute arguments manually. Use their return value in a decltype, which is typically a (member) function pointer.

* Separate template parameters: put the function into a standalone struct with a portion of the original parameters. Mostly useful if you need to have two argument packs in a single function.
