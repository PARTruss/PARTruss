
var gl;
var points;
var canvas;

window.onload = function init()
{
    canvas = document.getElementById( "gl-canvas" );
    
    gl = WebGLUtils.setupWebGL( canvas );
    if ( !gl ) { alert( "WebGL isn't available" ); }

    
    var xhr;
    if (window.XMLHttpRequest) {
        xhr = new XMLHttpRequest();
    } else if (window.ActiveXObject) {
        xhr = new ActiveXObject("Microsoft.XMLHTTP");
    }

    xhr.onreadystatechange = function(){
        if(xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200){
            drawTruss(xhr.responseText);
        }
    };
    xhr.open("GET","/data/example.json"); 
    xhr.send();

};

function drawTruss(raw){

    //alert(raw);

    var data = JSON.parse(raw);

    //alert(data["Vertices"]);
    //alert(data["Edges"]);



    var vertices = [];
    var edges = []; 

    for(var i=0;i<data.Vertices.length;i++){
    	vertices.push(vec3(data.Vertices[i].XYZPosition));
    }

    for(edge in data.Edges){
    	edges.push(data.Edges[edge].Endpoints);
    }

    //alert(edges.length);

    //
    //  Configure WebGL
    //
    gl.viewport( 0, 0, canvas.width, canvas.height );
    gl.clearColor( 0.0, 0.0, 0.0, 1.0 );
    
    //  Load shaders and initialize attribute buffers
    
    var program = initShaders( gl, "vertex-shader", "fragment-shader" );
    gl.useProgram( program );
    
    // Load the data into the GPU
    
    var bufferId = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(vertices), gl.STATIC_DRAW );

    edgeIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, edgeIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(flatten(edges)), gl.STATIC_DRAW);
	

    // Associate out shader variables with our data buffer
    
    var vPosition = gl.getAttribLocation( program, "vPosition" );
    gl.vertexAttribPointer( vPosition, 3, gl.FLOAT, false, 0, 0 );
    gl.enableVertexAttribArray( vPosition );

    gl.clear( gl.COLOR_BUFFER_BIT );

    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.drawArrays( gl.POINTS, 0, vertices.length);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, edgeIndexBuffer);
    gl.drawElements(gl.LINES, edges.length*2, gl.UNSIGNED_SHORT, 0)
}

/*function normalize(data){
    var mag=0.00;
    for(i=0; i<data.length; i++){
        if(data[i]>mag)
            mag=data[i];
        if((-data[i])>mag)
            mag=-data[i];
    }
    var out = [];
    for(i=0; i<data.length; i++){
        out.push(data[i]/mag);
    }
    return out;
}*/