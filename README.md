# Yalnix Project
## Ryan Gonzalez and Sam Hirsh

## Currently at checkpoint 4

Kernel functions work as expected(fork, exit, wait, exec)

### Connecting 

## Setup framework variable

Add this line to your `~/.bashrc`:

```bash
export YALNIX_FRAMEWORK="/thayerfs/courses/26spring/cosc058/workspace/yalnix_framework"
```

Reload `.bashrc`:

```bash
source ~/.bashrc
```

To confirm 


```bash
echo $YALNIX_FRAMEWORK
```
# Building

Make and make clean for rn very simple
Init.c has tests that will tests, fork, wait, exit, and exec calling itself or another program. 
Please comment and uncomment sections as testing.