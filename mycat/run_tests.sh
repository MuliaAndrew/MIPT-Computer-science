#!/bin/bash

./mycat tests/file1.txt tests/file2.txt tests/file3.txt tests/file4.txt tests/file5.txt tests/file6.txt tests/file7.txt tests/file8.txt tests/file9.txt > test_res.txt

code test_res.txt