# TODO

Functions and functionality to be implemented.

## `pebble.DB` Functions

```go
func (d *DB) LogData(data []byte, opts *WriteOptions) error
func (d *DB) RangeKeySet(start, end, suffix, value []byte, opts *WriteOptions) error
func (d *DB) RangeKeyUnset(start, end, suffix []byte, opts *WriteOptions) error
func (d *DB) RangeKeyDelete(start, end []byte, opts *WriteOptions) error
func (d *DB) Apply(batch *Batch, opts *WriteOptions) error
func (d *DB) NewBatch() *Batch
func (d *DB) NewIndexedBatch() *Batch
func (d *DB) NewIter(o *IterOptions) *Iterator
func (d *DB) NewSnapshot() *Snapshot
func (d *DB) Close() error
func (d *DB) Compact(start, end []byte, parallelize bool) error
func (d *DB) Flush() error
func (d *DB) Metrics() *Metrics
func (d *DB) SSTables(opts ...SSTablesOption) ([][]SSTableInfo, error)
func (d *DB) EstimateDiskUsage(start, end []byte) (uint64, error)
func (d *DB) FormatMajorVersion() FormatMajorVersion
func (d *DB) RatchetFormatMajorVersion(fmv FormatMajorVersion) error
func (d *DB) Ingest(paths []string) error
func (d *DB) IngestWithStats(paths []string) (IngestOperationStats, error)
```

### Excluded
Unneeded
```go
func (d *DB) NewIterWithContext(ctx context.Context, o *IterOptions) *Iterator
func (d *DB) AsyncFlush() (<-chan struct{}, error)
func (d *DB) Checkpoint(destDir string, opts ...CheckpointOption)
func (d *DB) CheckLevels(stats *CheckLevelsStats) error
```

Internal
```go
func (d *DB) ScanInternal(
	lower, upper []byte,
	visitPointKey func(key *InternalKey, value LazyValue) error,
	visitRangeDel func(start, end []byte, seqNum uint64) error,
	visitRangeKey func(start, end []byte, keys []keyspan.Key) error,
) error
```

Experimental
```go
func (d *DB) ApplyNoSyncWait(batch *Batch, opts *WriteOptions) error
func (d *DB) SetCreatorID(creatorID uint64) error
```

## `pebble.Batch` Functions

```go
func (b *Batch) Apply(batch *Batch, _ *WriteOptions) error
func (b *Batch) Get(key []byte) ([]byte, io.Closer, error)
func (b *Batch) Set(key, value []byte, _ *WriteOptions) error
func (b *Batch) Merge(key, value []byte, _ *WriteOptions) error
func (b *Batch) Delete(key []byte, _ *WriteOptions) error
func (b *Batch) SingleDelete(key []byte, _ *WriteOptions) error
func (b *Batch) DeleteRange(start, end []byte, _ *WriteOptions) error
func (b *Batch) RangeKeySet(start, end, suffix, value []byte, _ *WriteOptions) error
func (b *Batch) RangeKeyUnset(start, end, suffix []byte, _ *WriteOptions) error
func (b *Batch) RangeKeyDelete(start, end []byte, _ *WriteOptions) error
func (b *Batch) LogData(data []byte, _ *WriteOptions) error
func (b *Batch) Empty() bool
func (b *Batch) Len() int
func (b *Batch) Repr() []byte                           // needed?
func (b *Batch) SetRepr(data []byte) error              // needed?
func (b *Batch) NewIter(o *IterOptions) *Iterator
func (b *Batch) Commit(o *WriteOptions) error
func (b *Batch) Close() error
func (b *Batch) Indexed() bool
func (b *Batch) Reset()
func (b *Batch) SeqNum() uint64
func (b *Batch) Count() uint32
func (b *Batch) Reader() BatchReader                    // needed?
func (b *Batch) SyncWait() error                        // needed?
func (b *Batch) CommitStats() BatchCommitStats
```

### Unneeded

```go
func (b *Batch) SetDeferred(keyLen, valueLen int) *DeferredBatchOp
func (b *Batch) MergeDeferred(keyLen, valueLen int) *DeferredBatchOp
func (b *Batch) DeleteDeferred(keyLen int) *DeferredBatchOp
func (b *Batch) SingleDeleteDeferred(keyLen int) *DeferredBatchOp
func (b *Batch) DeleteRangeDeferred(startLen, endLen int) *DeferredBatchOp
func (b *Batch) RangeKeyDeleteDeferred(startLen, endLen int) *
func (b *Batch) NewIterWithContext(ctx context.Context, o *IterOptions) *Iterator
```

## `pebble.Iterator` Functions

```go
func (i *Iterator) SeekGE(key []byte) bool
func (i *Iterator) SeekGEWithLimit(key []byte, limit []byte) IterValidityState
func (i *Iterator) SeekPrefixGE(key []byte) bool
func (i *Iterator) SeekLT(key []byte) bool
func (i *Iterator) SeekLTWithLimit(key []byte, limit []byte) IterValidityState
func (i *Iterator) First() bool
func (i *Iterator) Last() bool
func (i *Iterator) Next() bool
func (i *Iterator) NextWithLimit(limit []byte) IterValidityState
func (i *Iterator) NextPrefix() bool
func (i *Iterator) Prev() bool
func (i *Iterator) PrevWithLimit(limit []byte) IterValidityState
func (i *Iterator) RangeKeyChanged() bool
func (i *Iterator) HasPointAndRange() (hasPoint, hasRange bool)
func (i *Iterator) RangeBounds() (start, end []byte)
func (i *Iterator) Key() []byte
func (i *Iterator) Value() []byte
func (i *Iterator) ValueAndErr() ([]byte, error)
func (i *Iterator) LazyValue() LazyValue
func (i *Iterator) RangeKeys() []RangeKeyData
func (i *Iterator) Valid() bool
func (i *Iterator) Error() error
func (i *Iterator) Close() error
func (i *Iterator) SetBounds(lower, upper []byte)
func (i *Iterator) SetOptions(o *IterOptions)
func (i *Iterator) Metrics() IteratorMetrics
func (i *Iterator) ResetStats()
func (i *Iterator) Stats() IteratorStats
func (i *Iterator) Clone(opts CloneOptions) (*Iterator, error)
```

### Unneeded
```go
func (i *Iterator) CloneWithContext(ctx context.Context, opts CloneOptions) (*Iterator, error)
```

## Other Structs/Options/Configs/Enums
* `BatchReader` - might be simple, just exposes a `Next(..)` function.
* `IterOptions` - large, unwieldy, likely needed
* `Snapshot` - seems simple, but has references that need to be explicitly surfaced
* `Metrics` - large, unwieldy 
* `SSTableInfo` - is this needed?  probably not 
* `BatchCommitStats` - probably not needed immediately, somewhat simple to expose 
* `IterValidityState` - simple enum 
* `LazyValue` - complex, not immediately needed 
* `RangeKeyData` - simple struct 
* `IteratorMetrics` - very simple struct, just contains an int for ReadAmp 
* `IteratorStats` - relatively simple struct 
* `CloneOptions` - should be simple but references IterOptions, unclear if needed at first

## Core Interfaces

```go
type Reader interface {
	Get(key []byte) (value []byte, closer io.Closer, err error)
	NewIter(o *IterOptions) *Iterator
	Close() error
}

type Writer interface {
	Apply(batch *Batch, o *WriteOptions) error
	Delete(key []byte, o *WriteOptions) error
	SingleDelete(key []byte, o *WriteOptions) error
	DeleteRange(start, end []byte, o *WriteOptions) error
	LogData(data []byte, opts *WriteOptions) error
	Merge(key, value []byte, o *WriteOptions) error
	Set(key, value []byte, o *WriteOptions) error
	RangeKeySet(start, end, suffix, value []byte, opts *WriteOptions) error
	RangeKeyUnset(start, end, suffix []byte, opts *WriteOptions) error
	RangeKeyDelete(start, end []byte, opts *WriteOptions) error
}

type iterator interface {
	SeekLT(key []byte) bool
	SeekGE(key []byte) bool
	Valid() bool
	Key() []byte
	Value() []byte
	First() bool
	Next() bool
	Last() bool
	Prev() bool
	Close() error
}

type batch interface {
	Close() error
	Commit(opts *pebble.WriteOptions) error
	Set(key, value []byte, opts *pebble.WriteOptions) error
	Delete(key []byte, opts *pebble.WriteOptions) error
	LogData(data []byte, opts *pebble.WriteOptions) error
}

```