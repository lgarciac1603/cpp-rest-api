# config.h

Add you local variables to DB connection

```cpp
#define DB_HOST "localhost" <== host (optional)
#define DB_PORT "5432" <== port (optional)
#define DB_NAME "apidb" <== name ()
#define DB_USER "" <== user
#define DB_PASS "" <== password
```

Docker compose includes test data that can be used to create a test instance of docker postgresql DB
