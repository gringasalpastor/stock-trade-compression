# Compression of Stock Trade Data Stream

Compresses a sequence of stock trades using a separate Huffman tree optimized for each data column
(price, time, symbol, etc) — Each value in the Huffman tree is pre-encoded as the difference between the
value of its prediction algorithm and its self to minimize variation. Outperforms generic Huffman coding based `bzip2 (77.8% vs. 90.4%)` by using data predictions specific to each data column.

## Build

```
mkdir build && cd build
cmake ..
make -j
```

## Compress and Decompress

```
# compress
./source/compress -i ../sample-trades/large-trades.csv -o output.bin

# decompress
./source/compress -d  -i ./output.bin -o ./output.csv
diff ../sample-trades/large-trades.csv output.csv # verify
```

## Unit Tests

```
make test
```

## Format
```
find ./ -name "*.hpp" -o -name "*.cpp" | xargs clang-format -i --style=file
```

## Metrics

Sample output for the provided 'large-trades.csv' file

```
./source/compress -i ../sample-trades/large-trades.csv -o output.bin
Input Size (bytes): 19113524
Output Size (bytes): 1834354
        All Header Data: 97133
                Symbol Header: 9895
                Exchange/Newline Header: 10
                Side Header: 19
                Condition Header: 9
                Send Time Header: 44548
                Receive Time Header: 16
                Price Header: 19908
                Price Overflow Header: 292
                Quantity Header: 22428
        Data: 1737221
Compression: 90.4028%
Compression (excluding header): 90.911%
```

> *NOTE*: Outperforms generic Huffman coding based `bzip2` (`77.8%` vs. `90.4%`) by using data predictions specific
to each data column
## Design

- We make 2 passes through the data. During both passes, after each field is read in, it gets mapped to a new symbol (prime value) based on the previous inputs. The first pass through the data calculates the frequency of each of these symbols (`symbol'`, `exchange_new_line'`, `side'`, `condition'`, `send_time'`, `receive_time_diff'`, `price'`, `overflow_price_digits'`, `quantity'`). These frequency maps are then turned into a best Huffman tree. For the best Huffman tree, we calculate the ideal size of each prefix value (prefix size map). The second pass is similar to the first. First we take the best Huffman prefixes and convert them to canonical best Huffman prefixes. Next we recalculate all of the primes (`symbol'`, `exchange_new_line'`, `side'`, `condition'`, `send_time'`, `receive_time_diff'`, `price'`, `overflow_price_digits'`, `quantity'`). The last byte will be padded out with 0's.

- Price is the absolute value of price field truncated to 2 decimals and multiplied 100 (example: `-3.1415 –-> 314`. `overflow_price_digits` is a string the has the first char as '-' if the value is negative, followed by the remaining truncated decimals values from the previous(example “-3.1415” –-> “-15”). Price values are stored this way to avoid floating point representation (For most trades, overflow_price_digits will be empty). This allows us to do math (like taking a difference) on the remaining integer part of the price.

- Exchange and new_line values were packed together to avoid storing a extra bit for the new_line field.

- Prediction Functions
  * `predict_send_time()` is the previous send_time plus a rolling average the the last 100 time gaps
  * `predict_receive_time_diff()`  is the previous send_time difference (`receive time - send time`) plus a rolling average the the last 100 send_time differences gaps (a double difference)
  * `predict_price()` is the previous price for a given symbol ticker
  * `predict_quantity()` is the previous quantity for a given symbol ticker


## Future Enhancements and Optimizations
- Concurrency – current implementation is single threaded
- Optimizations for encoding the bit sequences into bytes
- Use Huffman table on the Huffman table. I use fixed integer values to store the sizes of the prefix, this could be slightly smaller.
- [Burrows–Wheeler transform](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform)
- [Move-to-front transform](https://en.wikipedia.org/wiki/Move-to-front_transform)
- [Run-length encoding](https://en.wikipedia.org/wiki/Run-length_encoding)
