#!/usr/bin/env python3

import struct
from tabulate import tabulate


def main() -> None:
    with open("example.apkg.bin", "rb") as f:
        file_version = int.from_bytes(f.read(4), byteorder='little')
        asset_index_count = int.from_bytes(f.read(4), byteorder='little')
        data_start_pos = int.from_bytes(f.read(4), byteorder='little')

        # Prepare data for printing
        data = [
            ["File Version", f"{(file_version >> 16) & 0xFF}."
             f"{(file_version >> 8) & 0xFF}.{file_version & 0xFF}"],
            ["Asset Index Count", asset_index_count],
            ["Data Start Position", data_start_pos]
        ]

        # Print version, index size, and data start position
        print(tabulate(data, tablefmt="rounded_grid"))

        # Read and print each entry in the asset index tree
        index_list = []  # List to store index information
        for i in range(asset_index_count):
            entry = f.read(16)
            # Unpack into (id, start, size)
            entry_data = struct.unpack("<QII", entry)
            index_list.append(entry_data)

        # Prepare the data for tabulation
        table_data = []
        for i, entry_data in enumerate(index_list):
            table_data.append([i, entry_data[0], entry_data[1], entry_data[2]])

        # Print the table
        headers = ["Index", "ID", "Start", "Size"]
        print(tabulate(table_data, headers=headers, tablefmt="rounded_grid"))

        # Ask user to select an indexi
        selected_index = int(input("\nSelect an index to view its data: "))
        if 0 <= selected_index < asset_index_count:
            # Seek to the start position of the selected index's data
            f.seek(data_start_pos + index_list[selected_index][1])
            # Read the data of the selected index
            data = f.read(index_list[selected_index][2])
            print(data)
        else:
            print("Invalid index selected.")


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
