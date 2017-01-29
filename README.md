# htmlpdfconv (also known as html2pdf) [![Build status](https://ci.appveyor.com/api/projects/status/b9ei546kwt888rpr?svg=true)](https://ci.appveyor.com/project/gregfrazier/htmlpdfconv)
Converts HTML to PDF (barely)

This code is wretched and over 7 years old.
I'm making the internet a little dumber just having this crap here.

Basically, it's completely handwritten in C (with some C++ mixed in, so it needs a cpp compiler) features a junky tokenizer, a DOM tree implemented as a linked-list (yeah, seriously) and css/attribs to those elements are also linked lists. I went ham on linked lists. I used tidyhtml lib to fix bad html tags, haru pdf lib for the pdf drawing, and CXImage for image processing (converts all images to JPEG).

That's what a weekend of coding this as a demo for an ex-employer will get you. I did make enhancements, fixed up a bunch of stuff but unfortunately I made those enhancements while on the clock so this is the only version I can release (and the only version I have the code to.)

What does it support?
- HTML 3 maybe 4ish, no tables are drawn
- very little css, almost none. (background-color, color, font-size, background)
- very few attributes for elements, href, src, style (limited to above css), height, width, alt, face, color, bgcolor, size, align
- entities
- uhh... downloading images from internet as long as it's HTTP
- nearly any image that CXimage supports can be decoded, PNG files with alpha render as black
- crashing, it loves crashing.

How to use:
- Don't.
- It doesn't compile very well anymore, I would avoid
- Use it to learn how NOT to write code.
