import argparse
import io
import os

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('input', help="Name of input file")
    parser.add_argument('output', help="Name of output file")
    parser.add_argument("-n", "--name",
                        help="Name of C variable")
    args = parser.parse_args()
    return args

def main():
    args = get_args()
    with io.open(args.input, "rb") as input_file:
        try:
            with io.open(args.output, "wb") as output_file:
                output_file.write("static const char {}[] ={{\n".format(args.name).encode('utf-8'))

                linecount = 0
                while True:
                    byte = input_file.read(1)
                    if byte == b"":
                        break

                    output_file.write("0x{:02x}, ".format(ord(byte)).encode('utf-8'))
                    linecount = linecount + 1;
                    if linecount > 20:
                        output_file.write(b"\n ")
                        linecount = 0

                output_file.write(b"\n0")
                output_file.write(b"\n};\n\n")

        except Exception:
            os.unlink(args.output)
            raise

if __name__ == "__main__":
    main()
