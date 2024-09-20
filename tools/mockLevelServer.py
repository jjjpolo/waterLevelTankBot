from flask import Flask, send_from_directory, abort, jsonify
from flask_cors import CORS
import random
import os

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

# Route to serve index.html from the parent directory
@app.route('/')
def serve_index():
    parent_dir = os.path.abspath(os.path.join(os.getcwd(), os.pardir))  # Get the parent directory
    file_path = os.path.join(parent_dir, 'index.html')
    
    if os.path.exists(file_path):
        return send_from_directory(parent_dir, 'index.html')
    else:
        print(f"File not found: {file_path}")
        return abort(404)  # Return 404 if the file is not found

# Route to serve settings.html from the parent directory
@app.route('/settings')
def serve_settings():
    parent_dir = os.path.abspath(os.path.join(os.getcwd(), os.pardir))  # Get the parent directory
    file_path = os.path.join(parent_dir, 'settings.html')
    
    if os.path.exists(file_path):
        return send_from_directory(parent_dir, 'settings.html')
    else:
        print(f"File not found: {file_path}")
        return abort(404)  # Return 404 if the file is not found

# Endpoint to get the random level
@app.route('/level', methods=['GET'])
def get_random_level():
    level = random.randint(1, 100)
    return {'level': level}, 200

# Endpoint to get the random level
@app.route('/distance', methods=['GET'])
def get_random_distance():
    distance = random.randint(1, 100)
    return str(distance), 200

# Endpoint to get configuration settings in JSON format
@app.route('/getParameters', methods=['GET'])
def get_config():
    response = {
        "status": "ok",
        "maxDepth": 150,
        "minDepth": 20,
        "alarmTrigger": 25
    }
    return jsonify(response), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80)
