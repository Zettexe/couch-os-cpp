# Read the input JSON file
data=$(cat 'build/compile_commands.json')

# Extract the objects from the JSON array
objects=$(jq -c '.[]' <<< "$data")

# Print the opening square bracket
echo "[" > "build/compile_commands.json"

# Get the total number of objects
num_objects=$(echo "$objects" | wc -l)

# Initialize the counter
count=0

# Process each object
while read -r obj; do
  # Increment the counter
  count=$((count+1))

  # Extract the arguments field
  arguments=$(jq -r '.arguments | join(" ")' <<< "$obj")
  # Add the command field to the object
  obj=$(jq --arg cmd "$arguments" '. + {"command": $cmd}' <<< "$obj")

  # Print the modified object
  if [ "$count" -lt "$num_objects" ]; then
    echo "  $obj,"
  else
    echo "  $obj"
  fi

done <<< "$objects" >> "build/compile_commands.json"

echo "]" >> "build/compile_commands.json"
