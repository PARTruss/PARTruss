
var gl;
var canvas;
var data;

var submit, url, submit2, file;

var camera = {
    near: 0.01,
    far: 100,
    aspect: 0.7,
    fovAngle: 90,
    z: 4.0,
    theta: 0,
    phi: 0,
    dx:0,
    dy:0,
    dz:0,
}

var colorScale = {
    maxStress:0,
    minStress:0,
    minHue: 2/3.0,
    maxHue: -1/6.0,
    scale: 0,
    shift: 0,
}

var dr = 0.5 * Math.PI/180.0;

var vertices;

var isDown = false; // whether mouse is pressed
var startCoords = []; // 'grab' coordinates when pressing mouse
var last = [0, 0]; // previous coordinates of mouse release
var lastTheta = 0, lastPhi=0;
var lastx=0, lasty=0, lastz=0;

window.onload = function init()
{
    canvas = document.getElementById( "gl-canvas" );
    
    gl = WebGLUtils.setupWebGL( canvas );
    if ( !gl ) { alert( "WebGL isn't available" ); }
    gl.viewport( 0, 0, canvas.width, canvas.height );
    gl.clearColor( 0.0, 0.0, 0.0, 1.0 );
    
    url = document.getElementById( "url" );
    submit=document.getElementById( "submit" );
    submit.onclick=getAndDrawUrl;

    addEventListeners();

    getAndDrawUrl();

};

function addEventListeners(){
    canvas.onmousedown = function(e) {
        isDown = true;
        shift=e.shiftKey;

        startCoords = [
            e.offsetX , // set start coordinates
            e.offsetY
       ];
    };

    canvas.onmouseup   = function(e) {
        isDown = false;
        shift=false;
        last = [
            e.offsetX - startCoords[0], // set last coordinates
            e.offsetY - startCoords[1]
        ];

        lasxx=camera.dx;
        lasty=camera.dy;
        lastz=camera.dz;
        lastTheta=camera.theta;
        lastPhi=camera.phi;

    };

    canvas.onmousemove = function(e)
    {
        if(!isDown) return; // don't pan if mouse is not pressed
        if(shift!=e.shiftKey){
            canvas.onmouseup(e);
            canvas.onmousedown(e);
        }
        var x = e.offsetX;
        var y = e.offsetY;
        var xDiff=(x-startCoords[0])/100;
        var yDiff=(y-startCoords[1])/100;
        if(e.shiftKey){
            camera.dx=lastx-xDiff*Math.cos(camera.theta) - yDiff*Math.sin(camera.theta)*Math.sin(camera.phi);
            camera.dy=lasty+yDiff*Math.cos(camera.phi);
            camera.dz=lastz+xDiff*Math.sin(camera.theta) - yDiff*Math.cos(camera.theta)*Math.sin(camera.phi);
        }
        else{
            camera.theta=lastTheta-xDiff;
            camera.phi=lastPhi+yDiff;
        }
        render(); // render to show changes

    }

    var panning=false;

    document.addEventListener('wheel', function(event)
    {
        event.preventDefault();
        if (event.deltaY > 0) camera.z += 0.1;
        else camera.z -= 0.1;
        if(camera.z<0.1) camera.z=0.1;  
        render();  
    })

}


function getAndDrawUrl(){
    
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
    xhr.open("GET",url.value); 
    xhr.send();
}

function drawTruss(raw){

    data = JSON.parse(raw);

    setForceScale(data);

    vertices = [];

    for(var i=0;i<data.Vertices.length;i++){
    	vertices.push(vec3(data.Vertices[i].XYZPosition));
    }

    camera.z=zoomToFit(vertices);

    render();
}

function render(){
    var eye = [ Math.sin(camera.theta)*Math.cos(camera.phi)*camera.z + camera.dx,
                Math.sin(camera.phi)*camera.z + camera.dy, 
                Math.cos(camera.theta)*Math.cos(camera.phi)*camera.z + camera.dz] ;
    var at = [camera.dx, camera.dy, camera.dz];

    up = vec3(0.0, 1.0, 0.0);

    if(Math.cos(camera.phi)<0) {
        up[1]=-1;
    }

    modelViewMatrix = lookAt( eye, at, up );
    projectionMatrix = perspective(camera.fovAngle, camera.aspect, camera.near, camera.far);

    gl.clear( gl.COLOR_BUFFER_BIT );
    renderEdges();
    renderNodes();
    

}

function renderNodes(){
    var nodeprogram = initShaders( gl, "node-vertex-shader", "node-fragment-shader" );
    gl.useProgram( nodeprogram );

    var modelViewMatrixLoc = gl.getUniformLocation( nodeprogram, "modelViewMatrix" );
    var projectionMatrixLoc = gl.getUniformLocation( nodeprogram, "projectionMatrix" );

    gl.uniformMatrix4fv( modelViewMatrixLoc, false, flatten(modelViewMatrix) );
    gl.uniformMatrix4fv( projectionMatrixLoc, false, flatten(projectionMatrix) );
 
    var bufferId = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(vertices), gl.STATIC_DRAW );

    var vPosition = gl.getAttribLocation( nodeprogram, "vPosition" );
    gl.vertexAttribPointer( vPosition, 3, gl.FLOAT, false, 0, 0 );
    gl.enableVertexAttribArray( vPosition );

    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.drawArrays( gl.POINTS, 0, vertices.length);

}

function renderEdges(){
    var edgeprogram = initShaders( gl, "edge-vertex-shader", "edge-fragment-shader" );
    gl.useProgram( edgeprogram );

    var modelViewMatrixLoc = gl.getUniformLocation( edgeprogram, "modelViewMatrix" );
    var projectionMatrixLoc = gl.getUniformLocation( edgeprogram, "projectionMatrix" );
    
    gl.uniformMatrix4fv( modelViewMatrixLoc, false, flatten(modelViewMatrix) );
    gl.uniformMatrix4fv( projectionMatrixLoc, false, flatten(projectionMatrix) );

    var points = [];

    for(var i=0;i<data.Edges.length;i++){
        hue = data.Edges[i].Stress * colorScale.scale + colorScale.minHue;
        points.push(vec4(vertices[data.Edges[i].Endpoints[0]],hue));
        points.push(vec4(vertices[data.Edges[i].Endpoints[1]],hue));
    }

    var bufferId = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(points), gl.STATIC_DRAW );
    
    var vPosition = gl.getAttribLocation( edgeprogram, "vPosition" );
    gl.vertexAttribPointer( vPosition, 3, gl.FLOAT, false, 16, 0 );
    var hue = gl.getAttribLocation( edgeprogram, "hue" );
    gl.vertexAttribPointer( hue, 1, gl.FLOAT, false, 16, 12 );
    gl.enableVertexAttribArray( vPosition );
    gl.enableVertexAttribArray( hue );

    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.drawArrays( gl.LINES, 0, data.Edges.length*2);
}

function zoomToFit(data){
    var mag=0.00;
    var maxX, minX, maxY, minY, maxZ, minZ;
    maxX = minX = maxY = minY = maxZ = minZ = 0;
    for(i=0; i<data.length; i++){
        if(data[i][0]>maxX) maxX=data[i][0];
        if(data[i][0]<minX) minX=data[i][0];
        if(data[i][1]>maxY) maxY=data[i][1];
        if(data[i][1]<minY) minY=data[i][1];
        if(data[i][2]>maxZ) maxZ=data[i][2];
        if(data[i][2]<minZ) minZ=data[i][2];
    }
    mag=maxX-minX;
    if(mag < maxY-minY) mag = maxY-minY;
    if(mag < maxZ-minZ) mag = maxZ-minZ;
    mag=mag;
    return mag;
}

function setForceScale(data){
    colorScale.maxStress=0;
    colorScale.minStress=0;
    for(var i=0;i<data.Edges.length;i++){
        if(data.Edges[i].Stress > colorScale.maxStress)
            colorScale.maxStress = data.Edges[i].Stress;
        if(data.Edges[i].Stress < colorScale.minStress)
            colorScale.minStress = data.Edges[i].Stress;
    }

    if(colorScale.maxStress-colorScale.minStress!=0)
        colorScale.scale = 1/(colorScale.maxStress-colorScale.minStress);
    else
        colorScale.scale  = 1;
    colorScale.scale *= (colorScale.maxHue - colorScale.minHue);
    text=document.getElementById( "scaleData" );
    text.innerHTML = "Min stress(blue): "+colorScale.minStress+"<br/>"+
                     "Max stress(pink): "+colorScale.maxStress;
}