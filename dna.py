import random

def generate_random_dna_sequence(length):
    """Generates a random DNA sequence of the given length."""
    bases = ['A', 'T', 'C', 'G']
    return ''.join(random.choice(bases) for _ in range(length))

def main():
    try:
        # Ask the user for the desired sequence length
        length = int(input("Enter the length of the DNA sequence: "))

        if length <= 0:
            print("Length must be a positive integer.")
            return

        # Generate the DNA sequence
        dna_sequence = generate_random_dna_sequence(length)

        filename = "textfile/dna." + str(length) + ".txt"

        # Write the DNA sequence to a file
        with open(filename, "w") as file:
            file.write(dna_sequence)

        print(f"Random DNA sequence of length {length} has been written to dna.txt")

    except ValueError:
        print("Please enter a valid integer for the length.")

if __name__ == "__main__":
    main()
