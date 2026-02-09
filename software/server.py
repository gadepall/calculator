import socket
import ctypes
import os

so_file = os.path.abspath("libcalc.so") 
lib = ctypes.CDLL(so_file)

# void compute(const char* input, double start, double end, double step, char* output)
lib.compute.argtypes = [
    ctypes.c_char_p, 
    ctypes.c_double, 
    ctypes.c_double, 
    ctypes.c_double, 
    ctypes.c_char_p
]
lib.compute.restype = None

def process(expression):
    expression = expression.strip()
    
    #is_plot = 'x' in expression or 't' in expression
    
    input_str = expression.encode('utf-8')
    # Increased buffer size for plotting data
    out_buf = ctypes.create_string_buffer(65536) 
#    
#    if is_plot:
#        # Plot mode: 0 to 10 with step 0.5
#        # Change to implement param taking?
#        lib.compute(input_str, 0.0, 10.0, 0.5, output_buffer)
#    else:
#        # Scalar mode: Range doesn't matter, step=0 triggers single eval
    lib.compute(input_str, 0.0, 0.0, 0.0, out_buf)
    
    return out_buf.value

HOST = '127.0.0.1'
PORT = 65433

def start():
    print(f"[*] Calculator running on {HOST}:{PORT}")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen()
        
        while True:
            try:
                conn, addr = s.accept()
                with conn:
                    #print(f"[Connected] {addr}")
                    data = conn.recv(1024)
                    if not data:
                        break
                    expr = data.decode('utf-8')
                    print(f"[Received] {expr}")
                    
                    try:
                        response = process(expr)
                    except Exception as e:
                        response = f"Error: {e}".encode('utf-8')
                    
                    conn.sendall(response)
            except KeyboardInterrupt:
                print("\nStopping server...")
                break
            except Exception as e:
                print(f"Server Error: {e}")

if __name__ == "__main__":
    start()
