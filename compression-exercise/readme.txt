#How to build

cd /path/to/compression/
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

#How compress with the sample input file:

./compress -c ../test-files/cmeebat.csv ./output.bin

#How decompress with the sample input file:

./compress -d ./output.bin ./output.csv
diff ../test-files/cmeebat.csv output.csv 
<empty>

#How to run the tests:
make test

#To run each test stand alone:
./test_bits  
./test_csv_trade_source 
./test_encoder 
./test_huffman 
./test_trade

#Objects/Design

*encoder/decoder
At the highest level the main objects that one interacts with are the 'decoder' and the 'encoder'. The user level api for these is quite simple (encode_header(), decoder()). The decoder takes a pair of input and output streams. This was designed this way to allow for the possibility of making a test interface the streamed test cases to a decoder object. The encoder is more complicated and can't use streams, since it makes 2 passes on the input data. Both the encoder and decoder use the stats_collector object to make guesses as to what the coming values will be. Since, it uses the same prediction code path for both, that helps to ensure correct decoding.

*stats_collector
This object stores the model used to guess something close, given the past data. Originally, I played with using used an interpolation function to try to make better guesses, but simpler methods gave better results on the input file. The interpolation code has been moved to the 'playground' folder. stats_collector uses the 'symbol_data' class to store information on a per-symbol.

*huffman
This is utility collection of functions and structs for creating Huffman encodings. It can make a best tree, create a best prefix mapping, create canonical prefix mapping, and convert a best mapping to canonical form. All functions are templated based on the Symbol type, as to make it easy to switch from different symbols. This flexibility was very helpful when I realized that price values could have more than 3 digits (I just had to change the symbol type). It also makes it easy to have multiple header. I use this is create 9 headers.

*csv_source_range
csv_source_iterator inherits from boost::iterator_facade to easy make an iterator out of the input file. This lets you loop over trade values from an csv file using range based for-loops. This is flexible since other code could be build on top on any range of trade data. This object takes care of parsing each line in the file. Each trade is read lazily. It would be nice to add some internal buffering to this object, to speed up read times (reading bigger chunks of data).

*trade
Simple struct used for serializing trade data to and form files. 'to_csv' converts it to csv format.

*bits_to_bytes_converter(bits.hpp)
This is used for part of the Huffman algorithm, but mainly used for the encoding process. bits_to_bytes_converter takes in multiple sequences of bits and packs them into a vector of bytes. Bytes are stored in a `std::vector<uint8_t>`. This object makes it easy to throw lots of bits together, and turn that in to a sequence of bytes that be can written out to the disk.

*other
Mininal effort was put into encoding the header data. Programs like compress do smart things like but a huffman table of the huffman table. I use fixed integer values to store the sizes of the prefix, this could be smaller. Other optimizaions could of been done, like Burrows–Wheeler transform, move to front, and run-length encoding. I decided not to do these to keep the overall design simpler. run-length encoding was tempting me, but it would have made the design more complicated. 

#Metrics demonstrating the compression of the program
metrics are printed when you run the compress program.

Sample output for the provided 'cmeebat.csv' file

./compress -c ../test-files/cmeebat.csv ./output.bin
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

#Documentation on the algorithm and implementation

Compression
If the input file is empty (0 lines) the compressed and decompressed files will also be empty. 
The algorithm makes 2 passes through the data. During both passes, after each field is read in, it gets mapped to a new symbol(prime value) based on the previous inputs. When a line is read in it first populates the following data fields from 'trade' struct:

std::string symbol
std::pair<char, char> exchange_new_line
char side
char condition
int64_t send_time
int64_t receive_time
int32_t price
std::string& overflow_price_digits
int32_t quantity

Note
 *'exchange_new_line' is a pair of the exchange char and the type of newline for the given line ('\n' or '\r')
* price is the absolute value of price field truncated to 2 decimals and multiplied 100 (example “-3.1415” –-> “314”)
* overflow_price_digits is a string the has the first char as '-' if the value is negative, followed by the remaining truncated  decimals values from the previous(example “-3.1415” –-> “-15”).

Price values are stored this way to avoid floating point representation. For most trades, overflow_price_digits will be empty. This allows us to do math (like taking a difference) on the remaining integer part of the price.

Exchange and new_line values were packed together to avoid storing a extra bit for the new_line field. In retrospect, I think it would have been better to store them separately. If a field never changes, it has a size of 0; so the newline would of (in most cases) been better off as it's own field. However, this has little impact on the final compression.

The next step takes these 'trade' struct values and transforms them based on previous inputs.

The mapping is as follows:

symbol' = symbol
exchange_new_line' = exchange_new_line
side' = side
condition' = condition
send_time' = send_time - predict_send_time()
receive_time_diff' =  (receive_time – send_time) - predict_receive_time_diff()
price' = predict_price()
overflow_price_digits' = overflow_price_digits
quantity' = quantity – predict_quantity()

predict_send_time()
this is the previous send_time plus a rolling average the the last 100 time gaps

predict_receive_time_diff()
this is the previous send_time difference(receive time – send time) plus a rolling average the the last 100 send_time differences gaps (a double difference)

predict_price()
the previous price for a given symbol ticker

predict_quantity()
the previous quantity for a given symbol ticker


The first pass through the data calculates the frequency of each of these symbols (symbol', exchange_new_line', side', condition', send_time', receive_time_diff', price', overflow_price_digits', quantity'). These frequency maps are then turned into a best Huffman tree. For the best Huffman tree, we calculate the ideal size of each prefix value (prefix size map).

The header first writes out the total number of trades. Next each prefix size map is written out. The encode_table() function stores each symbol, followed by the size of it's prefix (32bit int value). to_bytes() converts each symbol to bytes. It has an overload for std::pair, std::string, and pod types. Strings are written out using a null terminator. The others are cast to bytes (this means files will not decompress if run on another machine with different endianness).

The second pass is similar to the first. First we take the best Huffman prefixes and convert them to canonical best Huffman prefixes. Next we recalculate all of the primes(symbol', exchange_new_line', side', condition', send_time', receive_time_diff', price', overflow_price_digits', quantity'). 

Now to encode a line we take each prime value and map it to it's canonical prefix. This prefix is appended to a 'bits_to_bytes_converter' object. After 'bits_to_bytes_converter' has 1 MG of  fully packed bytes, the data is flushed out(written to disk). At the end we flush the remaining data it 'bits_to_bytes_converter'. The last byte will be padded out with 0's.

Decompression

First we read the number of trades. Next we read all of the prefix tables. The tables only store the number of bits for each prime value, so we have to convert this to a canonical prefix encoding. 

Constructing the canonical prefix encoding
We sort all of the keys first by size and then by symbol order (each symbol has it's own ordering). Next for each key we append 0's to the last (prefix + 1) till we have the right size.

Next we read one bit at a time holding on the current prefix read. After reading each bit, we check if it is in the current map (if not we read another bit). After a prefix matches we write out the symbol and start with the empty prefix again. Note, symbols with only one table entry will just be the empty prefix.
