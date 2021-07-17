#include <stdio.h> 
#include <stdlib.h>

typedef unsigned long long __u64;

struct bkey {
	__u64	high;
	__u64	low;
	__u64	ptr;
};

#define BITMASK(name, type, field, offset, size)		\
static inline __u64 name(const type *k)				\
{ return (k->field >> offset) & ~(~0ULL << size); }		\
								\
static inline void SET_##name(type *k, __u64 v)			\
{								\
	k->field &= ~(~(~0ULL << size) << offset);		\
	k->field |= (v & ~(~0ULL << size)) << offset;		\
}

#define KEY_SIZE_BITS		16
#define KEY_MAX_U64S		8
#define MAX_KEY_SIZE		((1 << KEY_SIZE_BITS) - 1)

#define KEY_FIELD(name, field, offset, size)				\
	BITMASK(name, struct bkey, field, offset, size)

#define PTR_FIELD(name, offset, size)					\
static inline __u64 name(const struct bkey *k)		\
{ return (k->ptr >> offset) & ~(~0ULL << size); }

KEY_FIELD(KEY_PTRS,	high, 60, 3)
KEY_FIELD(HEADER_SIZE,	high, 58, 2)
KEY_FIELD(KEY_CSUM,	high, 56, 2)
KEY_FIELD(KEY_PINNED,	high, 55, 1)
KEY_FIELD(KEY_TYPE,	high, 37, 4)
KEY_FIELD(KEY_DIRTY,	high, 36, 1)

KEY_FIELD(KEY_SIZE,	high, 20, KEY_SIZE_BITS)
KEY_FIELD(KEY_INODE,	high, 0,  20)

#define KEY(inode, offset, size)					\
((struct bkey) {							\
	.high = (1ULL << 63) | ((__u64) (size) << 20) | (inode),	\
	.low = (offset)							\
})

#define ZERO_KEY			KEY(0, 0, 0)

#define MAX_KEY_INODE			(~(~0 << 20))
#define MAX_KEY_OFFSET			(~0ULL >> 1)
#define MAX_KEY				KEY(MAX_KEY_INODE, MAX_KEY_OFFSET, 0)

#define KEY_START(k)			(KEY_OFFSET(k) - KEY_SIZE(k))
#define START_KEY(k)			KEY(KEY_INODE(k), KEY_START(k), 0)

#define PTR_DEV_BITS			12

PTR_FIELD(PTR_DEV,			51, PTR_DEV_BITS)
PTR_FIELD(PTR_OFFSET,			8,  43)
PTR_FIELD(PTR_GEN,			0,  8)

#define PTR_CHECK_DEV			((1 << PTR_DEV_BITS) - 1)

#define MAKE_PTR(gen, offset, dev)					\
	((((__u64) dev) << 51) | ((__u64) offset) << 8 | gen)

int main(int argc, char *argv[])
{
	struct bkey key;

	if (argc < 4)
		return 0;

	sscanf(argv[1], "0x%llx", &(key.high));
	sscanf(argv[2], "0x%llx", &(key.low));
	sscanf(argv[3], "0x%llx", &(key.ptr));

	printf("0x%llx, 0x%llx, 0x%llx\n", key.high, key.low, key.ptr);
	printf("############################\n");
	printf("inode: %d\n", KEY_INODE(&key));
	printf("size: %d(sectors)\n", KEY_SIZE(&key));
	printf("dirty: %d\n", KEY_DIRTY(&key));
	printf("type: %d\n", KEY_TYPE(&key));
	printf("pinned: %d\n", KEY_PINNED(&key));
	printf("csum %d\n", KEY_CSUM(&key));
	printf("header size %d\n", HEADER_SIZE(&key));
	printf("ptrs %d\n", KEY_PTRS(&key));

	printf("\n");
	printf("ptr dev %d\n", PTR_DEV(&key));
	printf("ptr offset %d(sectors)\n", PTR_OFFSET(&key));
	printf("ptr gen %d\n", PTR_GEN(&key));
	printf("############################\n");

	return 0;
}
