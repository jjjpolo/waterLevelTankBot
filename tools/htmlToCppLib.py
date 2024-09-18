import os

def read_html_file(file_path):
    """Reads the content of an HTML file and returns it as a string."""
    if os.path.exists(file_path):
        with open(file_path, 'r', encoding='utf-8') as file:
            return file.read()
    else:
        print(f"Error: {file_path} does not exist.")
        return ""

def write_webservercontent_h(index_html_content, settings_html_content, output_file):
    """Generates the WebServerContent.h file with the HTML content from the files."""
    with open(output_file, 'w', encoding='utf-8') as header_file:
        # Write #pragma once and the namespace
        header_file.write("#pragma once\n\n")
        header_file.write("namespace WebServerContent {\n\n")
        
        # Add the content of index.html
        header_file.write('const char index_html[] PROGMEM = R"rawliteral(\n')
        header_file.write(index_html_content)
        header_file.write('\n)rawliteral";\n\n')

        # Add the content of settings.html
        header_file.write('const char settings_html[] PROGMEM = R"rawliteral(\n')
        header_file.write(settings_html_content)
        header_file.write('\n)rawliteral";\n\n')

        # Close the namespace
        header_file.write("}  // namespace WebServerContent\n")

def main():
    # Define paths to the HTML files in the parent directory
    base_dir = os.path.dirname(os.path.abspath(__file__))
    parent_dir = os.path.abspath(os.path.join(base_dir, "../"))
    index_html_path = os.path.join(parent_dir, 'index.html')
    settings_html_path = os.path.join(parent_dir, 'settings.html')
    
    # Read the content of the HTML files
    index_html_content = read_html_file(index_html_path)
    settings_html_content = read_html_file(settings_html_path)
    
    # Generate the WebServerContent.h file in the parent directory
    output_file = os.path.join(parent_dir, 'WebServerContent.h')
    write_webservercontent_h(index_html_content, settings_html_content, output_file)
    
    print(f"File {output_file} successfully generated.")

if __name__ == "__main__":
    main()
