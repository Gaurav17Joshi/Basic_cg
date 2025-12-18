from flask import Flask, request, send_file, render_template
import subprocess
import os

app = Flask(__name__)

@app.route('/')
def index():
    return render_template("index.html")

@app.route('/render')
def render():
    cam_x = request.args.get('x', '0')
    cam_y = request.args.get('y', '0')
    cam_z = request.args.get('z', '0')

    # Run the C++ executable with camera parameters
    # Note: The executable is now in the same directory as the server
    subprocess.run([
        "./hosting_executable",
        "--cam_x", cam_x,
        "--cam_y", cam_y,
        "--cam_z", cam_z
    ], check=True)
    
    # Return the generated image
    return send_file("output.png", mimetype='image/png')

if __name__ == '__main__':
    # Generate the initial image so it's ready on first load
    print("Generating initial image...")
    subprocess.run(["./hosting_executable"], check=True)
    print("Starting server...")
    app.run(debug=True, port=5000)