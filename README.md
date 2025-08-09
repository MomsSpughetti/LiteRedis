

# LiteRedis

A lightweight, embeddable Redis-style key-value store written in C++.  
No daemon, no configuration, no external dependencies — just drop it into your app and go.

---

## 🚀 Why LiteRedis?

LiteRedis is designed for developers who love Redis-style data access but don’t want to spin up a server for every project. Whether you're building a CLI tool, an offline desktop app, or prototyping fast logic, LiteRedis gives you:

- In-memory storage with Redis-like semantics
- TTL (time-to-live) support for expiring keys
- Optional snapshot-based persistence
- Simple C++ API or command-line interface
- Zero configuration, embeddable as a library

---

## 🧠 Use Cases

- Terminal-based productivity apps (CRM, note trackers, offline task managers)
- Local development tools needing fast state/caching
- Embedded systems where Redis is overkill
- Educational tools for learning database internals

---

## 🔍 Feature Comparison

| Feature                      | Redis Server | SQLite     | RocksDB    | LMDB       | **Redis Light** |
|-----------------------------|--------------|------------|------------|------------|-----------------|
| Embeddable                  | ❌ Daemon    | ✅         | ✅         | ✅         | ✅ ✅ ✅         |
| In-Memory Mode              | ✅           | ⚠️ Disk    | ⚠️ Mostly  | ✅         | ✅              |
| TTL Support                 | ✅           | ❌         | ❌         | ❌         | ✅              |
| Redis API / Command Format  | ✅           | ❌         | ❌         | ❌         | ✅              |
| Persistence (Optional)      | ✅ (RDB, AOF)| ✅         | ✅         | ✅         | ✅ (snapshot)   |
| External Dependencies       | Redis Server| SQLite Lib | RocksDB Lib| LMDB Lib   | None            |
| Suited for CLI Tools        | ⚠️ Server    | ✅         | ✅         | ✅         | ✅ ✅ ✅         |
| Zero Config / Portable      | ❌           | ✅         | ⚠️ Some    | ✅         | ✅ ✅ ✅         |

---

## 🛠️ Example Usage

### C++ (Library)

```cpp
#include "redis_light.h"

RedisLight db;
db.set("foo", "bar");
std::string value = db.get("foo");  // "bar"
db.expire("foo", 10);               // Expires in 10 seconds
db.save("snapshot.rdb");
````

### CLI

```bash
$ ./redislight-cli SET foo bar
OK
$ ./redislight-cli GET foo
bar
$ ./redislight-cli EXPIRE foo 60
OK
```

---

## 📦 Build & Installation

### Library

```bash
make lib
```

Produces `libredislight.a`

### CLI Tool

```bash
make cli
```

Produces `./redislight-cli`

### Include in Your Project

```cpp
#include "redis_light.h"
```

No external dependencies required.

---

## 🧪 Features Roadmap

-  Basic Redis commands: `SET`, `GET`, `DEL`, `EXPIRE`
    
-  TTL eviction
    
-  Snapshot persistence
    
-  Hashes and Lists support
    
-  Pub/Sub (optional)
    
-  LRU cache mode
    

---

## 📜 License

MIT License.

---

## 👤 Author

Built by Abdalla Egbareia, Computer Science gradute passionate about systems programming and developer tooling.

