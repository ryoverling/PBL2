# SQL Server schema for MoneyManager

These scripts are for SQL Server Express with Windows Authentication. They do
not read or import any `.dat` file. They do not drop a database or a table.

## Before running

Use a Windows account with permission to create the dedicated test database.
For this isolated, same-computer Stage A test only, the project owner permits
`Encrypt=Yes;TrustServerCertificate=Yes`. With `sqlcmd`, `-Nm` requires
encryption and `-C` skips certificate validation. This still encrypts the
connection, but does **not** authenticate the server's identity. Do not use
this exception for a production application or production schema deployment.
For production, install/bind a SQL Server certificate whose name matches the
client's server name and whose issuing chain Windows trusts, grant the SQL
Server service access to its private key, and then use `Encrypt=Yes` with
`TrustServerCertificate=No` (no `sqlcmd -C`).

Check the secure connection first:

```powershell
$server = "$env:COMPUTERNAME\SQLEXPRESS"
sqlcmd -S $server -E -Nm -C -b -Q "SELECT @@SERVERNAME, ORIGINAL_LOGIN(), encrypt_option FROM sys.dm_exec_connections WHERE session_id=@@SPID"
```

Then, from the repository root:

```powershell
sqlcmd -S $server -E -Nm -C -b -f 65001 -d master -i 'sql\000_create_test_database.sql'
sqlcmd -S $server -E -Nm -C -b -f 65001 -d MoneyManager_StageA_Test -i 'sql\001_init_schema.sql'
sqlcmd -S $server -E -Nm -C -b -f 65001 -d MoneyManager_StageA_Test -i 'sql\002_schema_smoke_test.sql'
```

`000` refuses to run if the test database already exists. `001` and `002`
refuse to run outside `MoneyManager_StageA_Test`; `001` also refuses to run
against a database containing user tables. `002` inserts
temporary test records inside a transaction and rolls them back. Inspect the
actual output and exit codes before treating any check as passed.

The Qt SQL probe in `tests/sql_connection_probe.cpp` uses ODBC Driver 18,
Windows Authentication, and `Encrypt=Yes`. Certificate validation remains on
by default. To exercise the same-computer Stage A exception explicitly, pass
`--local-test-trust-server-certificate` after the server and database arguments;
the program refuses this option for any other server or database. It is a
separate test program and is not linked into the GUI.

On the current Qt 6.12.0 / MinGW 64-bit development machine, build and run it
from the repository root with:

```powershell
& 'C:\Qt\Tools\CMake_64\bin\cmake.exe' -S tests/sql_connection_probe -B build/stage_a_probe -G Ninja -DCMAKE_PREFIX_PATH='C:\Qt\6.12.0\mingw_64' -DCMAKE_MAKE_PROGRAM='C:\Qt\Tools\Ninja\ninja.exe' -DCMAKE_CXX_COMPILER='C:\Qt\Tools\mingw1310_64\bin\g++.exe'
& 'C:\Qt\Tools\CMake_64\bin\cmake.exe' --build build/stage_a_probe
$env:Path = 'C:\Qt\6.12.0\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;' + $env:Path
& 'build\stage_a_probe\sql_connection_probe.exe' $server master '--local-test-trust-server-certificate'
```

The probe must print `Connection encrypted: TRUE`. Later, after creating the
test database, pass `MoneyManager_StageA_Test` instead of `master`. The test
scripts intentionally roll back their rows; SQL Server identity counters can
still advance after a rollback, which does not mean rows remain.
