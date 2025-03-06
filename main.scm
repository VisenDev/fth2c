(define fp (open-input-file "test.fth"))

(define (get-token input-port)
    (let ((token ""))

      (set token (string-append token 
