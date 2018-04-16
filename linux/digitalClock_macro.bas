' A simple Open Office Impress macro to display a digital clock on slides [2,n-1]

sub updateClocks
	' Desired position and the size of the clock
	' The default setting places a tight box at the lower right corner of the slide
	Dim Point As New com.sun.star.awt.Point
	Dim Size As New com.sun.star.awt.Size
	Point.x = 24000 'horizontal position
	Point.y = 19000 'vertical position
	Size.Width = 4000
	Size.Height = 2000

	Dim Doc As Object
	Doc = ThisComponent

	Dim oSlides As Object
	oSlides = Doc.getDrawPages()
	Dim numberOfSlides as Integer
	numberOfSlides = oSlides.getCount()

	' Place a separate clock on every single page except the first page
	for i=1 to numberOfSlides-1
		Dim RectangleShape As Object
		' Prepare a black rectangular box for the clock body
		RectangleShape = Doc.createInstance("com.sun.star.drawing.RectangleShape")
		RectangleShape.Size = Size
		RectangleShape.Position = Point
		RectangleShape.FillColor = RGB(0,0,0)
		RectangleShape.LineColor = 0
		
		Dim Page As Object
		Page = Doc.DrawPages(i)
		Page.add(RectangleShape)
	    
	    ' Generate a red digital text indicating current time (HH:MM in 24h system).
	    text2print = Right("00" & Hour(now), 2) & ":" & Right("00" & Minute(now), 2)
	  	RectangleShape.String = text2print
	  	RectangleShape.CharFontName = "Ani"
		RectangleShape.CharWeight = com.sun.star.awt.FontWeight.BOLD
		RectangleShape.CharHeight = 32
		RectangleShape.CharColor = RGB(255,0,0)
	next i
end sub


Sub showClock
	while true
		' Update the digital clock every 30*1000ms = 30 seconds.
		updateClocks
		wait (3e4)
	Wend	
end sub
