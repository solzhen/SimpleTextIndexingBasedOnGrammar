import re

def parse_file(file_path):
    pattern_occurrences = []

    # Open the file with ISO-8859-1 encoding
    with open(file_path, 'r', encoding='iso-8859-1') as file:
        content = file.read()

        regex = r'PATTERN:\{(.*?)\},\s*OCCS:\{(\d+)\}'
        matches = re.findall(regex, content, re.DOTALL)

        for match in matches:
            pattern = match[0].strip()
            occurrences = int(match[1])
            # Check if the pattern spans multiple lines (contains newline characters)
            if '\n' not in pattern:
                pattern_occurrences.append((pattern, occurrences))

    sorted_patterns = sorted(pattern_occurrences, key=lambda x: x[1], reverse=True)
    return sorted_patterns

def group_patterns_by_occurrences(sorted_patterns, range_diff=1):
    # List to hold the grouped patterns
    grouped_patterns = []
    
    # Temporary group to collect patterns with similar occurrences
    current_group = []
    last_occurrence = None

    for pattern, occurrences in sorted_patterns:
        # If it's the first pattern or the difference with the last occurrence is within the range
        if last_occurrence is None or abs(occurrences - last_occurrence) <= range_diff:
            current_group.append((pattern, occurrences))
        else:
            # When the difference exceeds the range, add the current group to the result
            grouped_patterns.append(current_group)
            current_group = [(pattern, occurrences)]  # Start a new group
        
        # Update last_occurrence
        last_occurrence = occurrences

    # Add the last group (in case it wasn't added)
    if current_group:
        grouped_patterns.append(current_group)

    return grouped_patterns

# Example usage
file_path = 'textfile/einstein.en.txt.patterns.txt'
pattern_occurrences = parse_file(file_path)

# Print the extracted pairs
""" for pattern, occurrences in pattern_occurrences:
    print(f"Pattern: {pattern}\nOccurrences: {occurrences}\n")
 """
grouped_patterns = group_patterns_by_occurrences(pattern_occurrences, range_diff=40)

# Print the groups
for group in grouped_patterns:
    print("Group of similar occurrences:")
    for pattern, occurrences in group:
        print(f"Pattern: {pattern}, Occurrences: {occurrences}")
    print("\n")
# print number of groups
print(f"Number of groups: {len(grouped_patterns)}")

out_folder = 'textfile/patterns/'
for group in grouped_patterns:
    first_occurrence_count = group[0][1]  # Occurrences are at index 1 of each (pattern, occurrences) pair
    pattern_file_name = f"{out_folder}group_{first_occurrence_count}.txt"
    with open(pattern_file_name, 'w') as file:
        for pattern, occurrences in group:
            file.write(f"{pattern}\n")
