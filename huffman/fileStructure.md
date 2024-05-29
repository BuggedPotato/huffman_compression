# compressed file structure
- treeSize is size of the compression tree in bytes (4B)
- elementSize is saved sequence size in bytes (1B)
## check bit by bit (tree saved in preorder)
### if 1
	- create a parenting node and attach to previous one
### if 0
	- create a leaf
	- read elementSize bytes and save to leaf
	- attach leaf to parent

## raw data to decode