## Repository Structure

*   `My codes/book_code/`: Contains the foundational C++ code directly adapted from the "Ray Tracing in One Weekend" book. This code is kept separate and frozen to maintain its integrity as a reference.
*   `My codes/web_app/`: Houses the interactive web application.
    *   `hosting_main.cc`: The C++ backend for the web app, responsible for rendering images based on parameters received from the server.
    *   `server.py`: A Python Flask server that handles web requests, triggers the C++ renderer, and serves the rendered images and web interface.
    *   `templates/index.html`: The frontend HTML structure and JavaScript logic for the interactive UI.
    *   `static/style.css`: Styling for the web interface.
    *   `book_headers/`: A local copy of the essential header files from `My codes/book_code`, making the `web_app` self-contained.
    *   `stb_image_write.h`: A single-file header library for writing PNG images.
*   `assets/images/`: Contains images relevant to the project, such as application screenshots.