from flask import Flask, request, send_file, render_template, jsonify
import subprocess
import os
import atexit
import signal
import threading

app = Flask(__name__)
render_process = None
render_lock = threading.Lock() # To prevent race conditions

RENDER_COMPLETE_FILE = "render_complete.tmp"
RENDER_STATUS_FILE = "render_status.json"

def kill_proc(proc):
    if proc and proc.poll() is None:
        print(f"Terminating running process (PID: {proc.pid})")
        try:
            os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
            proc.wait() # Wait for the process to actually terminate
        except ProcessLookupError:
            pass # Process already gone

atexit.register(lambda: kill_proc(render_process))

@app.route('/')
def index():
    return render_template("index.html")

@app.route('/start_render')
def start_render():
    global render_process
    
    with render_lock: # Ensure only one thread can execute this block at a time
        kill_proc(render_process)

        if os.path.exists(RENDER_COMPLETE_FILE):
            os.remove(RENDER_COMPLETE_FILE)
        if os.path.exists(RENDER_STATUS_FILE):
            os.remove(RENDER_STATUS_FILE)

        args = [
            "./hosting_executable",
            "--cam_x", request.args.get('x', '0'),
            "--cam_y", request.args.get('y', '0'),
            "--cam_z", request.args.get('z', '0'),
            "--vfov", request.args.get('vfov', '20'),
            "--samples", request.args.get('samples', '10'),
            "--depth", request.args.get('depth', '10'),
            "--width", request.args.get('width', '400'),
            "--aspect", request.args.get('aspect', '1.777')
        ]
        
        print(f"Starting new render with command: {' '.join(args)}")
        render_process = subprocess.Popen(args, preexec_fn=os.setsid)
    
    return jsonify(status="started", pid=render_process.pid)

@app.route('/get_image')
def get_image():
    return send_file("output.png", mimetype='image/png', as_attachment=False)

@app.route('/render_status')
def render_status():
    if os.path.exists(RENDER_COMPLETE_FILE):
        return jsonify(status="completed")
    
    if os.path.exists(RENDER_STATUS_FILE):
        with open(RENDER_STATUS_FILE, 'r') as f:
            try:
                data = f.read()
                if data:
                    import json
                    return jsonify(json.loads(data))
            except (IOError, json.JSONDecodeError):
                return jsonify(status="initializing")

    return jsonify(status="initializing")

if __name__ == '__main__':
    print("Starting server on port 5001...")
    app.run(debug=False, port=5001)
