# Summary #

  * Coding with ffdshow<p />
> Status:
    1. Finish building with our lib.
    1. The building one can decode video and send data to backend in Host.
  * Coding with ffmpeg<p />
> Status:
    1. Write a proxy, can receive data from socket and decode it.
    1. Write back Decoded data, processing
  * Establish socket communication lib<br>
</li></ul><blockquote>Status: Using tcp and virtio, bandwith 40mb/s,not so effective but can be accepted.
  * Design communication protocol <a href='http://code.google.com/p/vm-playback-driver/downloads/detail?name=VMACC_Protocol_Draft.doc&can=2&q='>view</a> <br>
</blockquote>> Status: We made a draft, leave some struct TBD.



&lt;hr&gt;


# Next Week Plan #

  * Complete Protocol
  * Link Frontend with Backend by Protocol,for backend can decode video from frontend and front end can playback video frame from backend.