# Building and Testing sqlpp17

## Building

To build sqlpp17 without any connector run:
```shell
cmake -B build
cmake --build build
```

Variables to build connector: 
* `MYSQL_CONNECTOR`
* `POSTGRESQL_CONNECTOR`
* `SQLITE3_CONNECTOR`
* `SQLCIPHER_CONNECTOR`

```shell
cmake -B build -DMYSQL_CONNECTOR=ON -DPOSTGRESQL_CONNECTOR=ON
cmake --build build
```

## Testing

Tests will be build by default and can be run with: 

```shell
ctest --test-dir build
```

## CMakeUserPreset

For convenience you can define a `CMakeUserPresets.json` if you have a recent cmake version. 

For example: 
```json 
{
  "version": 2,
  "configurePresets": [
      {
          "name": "default",
          "generator": "Ninja",
          "binaryDir": "build",
          "cacheVariables": {
              "CMAKE_EXPORT_COMPILE_COMMANDS": "1",
              "MYSQL_CONNECTOR": "1",
              "POSTGRESQL_CONNECTOR": "1",
              "SQLITE3_CONNECTOR": "1",
              "SQLCIPHER_CONNECTOR": "1"
          }
      }
  ],
  "buildPresets": [
      {
          "name": "default",
          "configurePreset": "default"
      }
  ],
  "testPresets": [
      {
          "name": "default",
          "configurePreset": "default"
      }
  ]
}
```

To build and test:

```shell
cmake --preset="default"
cmake --build --preset="default"
ctest --preset="default"
```

