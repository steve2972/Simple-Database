# Database Management Systems

## Project 1
Pokemon Database => practice with MySQL and basic operations

<br>

---

## Project 2

Created a simple on-disk database that allows the user to do the following:

<br>

### What this project does

- Insert a **key** and **value**
- Find the **value** corresponding to a **key**
- Delete a **Record** corresponding to a **key**
- **Open and manage a simple database file**


<br>

### How To use

> First, build an environment where we can create our file

```
git clone <this site>
cd 2019_ITE2038_2016054148/project2

make clean
make
```

This will clean any clutter in the workspace and create a library file where
all the API's are stored.

> Database API's are as follows (can be used in the main.c function)

```
db_open(char * pathname);
db_insert(int64_t key, char * value);
db_find(int64_t key);
db_delete(int64_t key);
```

For more information on how this program works, please check my [Wiki](https://hconnect.hanyang.ac.kr/2019_ITE2038_12230/2019_ITE2038_2016054148/wikis/milestone-2)