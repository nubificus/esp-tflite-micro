import os
import socket

IMAGE_SIZE = 9216

# Create a directory and place inside files from 
# https://github.com/nubificus/esp-tflite-micro/tree/master/examples/person_detection/static_images/sample_images
# and provide the path to the following variable. 
# The Server then will send the images sequentially 
# (one for every request).

IMAGE_DIR = "/home/ilias/Desktop/dataset/" 


def load_images(image_dir):
    images = []
    for filename in sorted(os.listdir(image_dir)):
        with open(os.path.join(image_dir, filename), 'rb') as f:
            images.append(f.read())
    return images

def main():
    server_ip = '0.0.0.0'
    server_port = 1234
    images = load_images(IMAGE_DIR)
    image_count = len(images)
    
    if image_count == 0:
        print("No images found in the directory")
        return
    
    print(f"Loaded {image_count} images.")

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((server_ip, server_port))
    server_socket.listen(1)
    print(f"Server listening on {server_ip}:{server_port}")

    conn, addr = server_socket.accept()
    print(f"Connection from {addr}")

    image_index = 0
    try:
        while True:
            request = conn.recv(1)
            if not request:
                break
            if request == b'\x01':  # Request byte from the client
                conn.sendall(images[image_index])
                print(f"Sent image {image_index + 1}/{image_count}")
                image_index = (image_index + 1) % image_count
    except Exception as e:
        print(f"Exception: {e}")
    finally:
        conn.close()
        server_socket.close()

if __name__ == "__main__":
    main()

