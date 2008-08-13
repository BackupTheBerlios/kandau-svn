function createList(id)
{
	list = new List()
	list.node=document.getElementById( id )
	list.paint()
	widgets[ id ] = list
	//var timeout = setTimeout( "list.paint()", 2000 )
}

function List()
{
	this.node=null
	this.paint=paint
	this.setHeader=setHeader
	this.setItems=setItems
	this.exampleData=exampleData
	this.header = new Array()
	this.items = new Array()
}

function exampleData()
{
	// alert( "exampleData" )
	this.header[ 0 ] = "Title"
	this.header[ 1 ] = "ISBN"
	this.header[ 2 ] = "Year"
	this.header[ 3 ] = "Author"

	this.items[ "Benzina" ] = new Array()
	this.items[ "Benzina" ][ 0 ] = "Benzina"
	this.items[ "Benzina" ][ 1 ] = "84-7727-434-7"
	this.items[ "Benzina" ][ 2 ] = "1983"
	this.items[ "Benzina" ][ 3 ] = "Quim Monzó"

	this.items[ "Sentimental" ] = new Array()
	this.items[ "Sentimental" ][ 0 ] = "Sentimental"
	this.items[ "Sentimental" ][ 1 ] = "85-7727-434-7"
	this.items[ "Sentimental" ][ 2 ] = "1980"
	this.items[ "Sentimental" ][ 3 ] = "Sergi Pàmies"
}

function setHeader( head )
{
	this.header = header
}

function setItems( it )
{
	this.items = it
}

function paint()
{
	var html
	html="<table>"
	html = html + "<tr>"
		for ( i in this.header ) {
		html = html + "<th>" + this.header[i] + "</th>"
		}
		html = html + "</tr>"

	//html=html + "<tr><th>Title</th><th>ISBN</th><th>Year</th><th>Author</th></tr>"
	for (i in this.items ) {
		html = html + "<tr>"
		for (j in this.header) {
			html = html + "<td>"
			html = html + this.items[ i ][ j ]
			html = html + "</td>"
		}
		html = html + "</tr>"
	}
	html=html + "</table>"
	this.node.innerHTML=html
}
