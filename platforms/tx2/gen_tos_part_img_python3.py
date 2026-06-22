#!/usr/bin/env python3
import sys
from pathlib import Path


def replace(text, old, new):
    return text.replace(old, new)


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: gen_tos_part_img_python3.py <gen_tos_part_img.py>")

    path = Path(sys.argv[1])
    text = path.read_text()

    replacements = [
        ("#!/usr/bin/python", "#!/usr/bin/env python3"),
        ('print "--tostype specified without --os"', 'print("--tostype specified without --os")'),
        ('print "Invalid --tostype " + args.tostype', 'print("Invalid --tostype " + args.tostype)'),
        (
            'print "Generating Trusted OS Partition Image File (encrypting input)"',
            'print("Generating Trusted OS Partition Image File (encrypting input)")',
        ),
        (
            'print "Generating Trusted OS Partition Image File (pre-encrypted input)"',
            'print("Generating Trusted OS Partition Image File (pre-encrypted input)")',
        ),
        (
            'print "Generating Trusted OS Partition Image File"',
            'print("Generating Trusted OS Partition Image File")',
        ),
        (
            'print "Generate TOS Image File for boot-wrapper."',
            'print("Generate TOS Image File for boot-wrapper.")',
        ),
        (
            'iv="00000000000000000000000000000000".decode("hex")',
            'iv = bytes.fromhex("00000000000000000000000000000000")',
        ),
        ("iv_hex=\"00000000000000000000000000000000\"\niv = bytearray.fromhex(iv_hex).decode('utf-8')",
         "iv_hex=\"00000000000000000000000000000000\"\niv = bytes.fromhex(iv_hex)"),
        ('iv=args.iv.decode("hex")', 'iv = bytes.fromhex(args.iv)'),
        ("str(\"NVTOSP\"),", "str(\"NVTOSP\").encode('utf-8'),"),
        ("str(img_size + img_align),", "str(img_size + img_align).encode('utf-8'),"),
        ("iv.encode('utf-8'),", "iv,"),
        ("header = '\\0' * (512-s.size)", "header = b'\\0' * (512-s.size)"),
        ("dest = open(output_name, 'w')", "dest = open(output_name, 'wb')"),
        ("dest = open(img_out, 'w')", "dest = open(img_out, 'wb')"),
        ("dest.write(header.encode('utf-8'))", "dest.write(header)"),
        ("dest.write('\\0' * tos_align)", "dest.write(b'\\0' * tos_align)"),
        ("dest.write('\\0' * img_align)", "dest.write(b'\\0' * img_align)"),
        ('"00000000000000000000000000000001".decode("hex")',
         'bytes.fromhex("00000000000000000000000000000001")'),
        ('key=k.read().encode("hex")', "key=binascii.hexlify(k.read()).decode('ascii')"),
        ("image_file.write('\\0' * (tos_size - tos_size_nopad))",
         "image_file.write(b'\\0' * (tos_size - tos_size_nopad))"),
        ('iv.encode("hex")', "binascii.hexlify(iv).decode('ascii')"),
    ]

    for old, new in replacements:
        text = replace(text, old, new)

    path.write_text(text)


if __name__ == "__main__":
    main()
