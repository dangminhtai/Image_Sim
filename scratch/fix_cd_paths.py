import os

filepath = "data/features_CD.yml"
print(f"Fixing paths in: {filepath}")
with open(filepath, 'r', encoding='utf-8') as f:
    content = f.read()

# Replace the absolute path with the new relative path under data/CD
old_path_windows_escaped = "f:\\\\X-FILE\\\\Code_UNI\\\\C++\\\\Image_Improvements\\\\data\\\\training_images\\\\"
old_path_windows_escaped_upper = "F:\\\\X-FILE\\\\Code_UNI\\\\C++\\\\Image_Improvements\\\\data\\\\training_images\\\\"
new_path_escaped = "data\\\\CD\\\\training_images\\\\"

content = content.replace(old_path_windows_escaped, new_path_escaped)
content = content.replace(old_path_windows_escaped_upper, new_path_escaped)

# Just in case there are single slashes
content = content.replace("data/training_images/", "data/CD/training_images/")
content = content.replace("data\\\\training_images\\\\", "data\\\\CD\\\\training_images\\\\")

with open(filepath, 'w', encoding='utf-8') as f:
    f.write(content)
print("Done fixing features_CD.yml.")
