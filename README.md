# Yalnix Project
## Ryan Gonzalez and Sam Hirsh

## Currently at checkpoint 4

Kernel functions work as expected.
One minor bug in fork, which is that the child resumes at parents delay, which then causes segfault. 
Test 2 and Test 3 are buggy because of the Fork bug. Currently working on fixing this
Also getting a warning about stale TLB so need to find where we are misssing a flush

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