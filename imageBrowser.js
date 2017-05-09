
let buttonSkin = new Skin ({fill: 'green'});
let whiteSkin = new Skin ({fill: 'white'});
let blueSkin = new Skin ({fill: '#3498db'});
let silverSkin = new Skin ({fill: '#bdc3c7'});

let buttonStyle = new Style({font: '22px', color: 'white'});
let headlineStyle = new Style({font: '28px', color: '#aaaaaa'});
let smallStyle = new Style ({font: '20px', color: 'black'});

var url = 'http://xkcd.com/info.0.json';

let MainContainer = Column.template($ => ({
    left: 0, right: 0, top: 0, bottom: 0, skin: whiteSkin,
    contents: [
      new LoadButton(),
      new ComicPane(),
    ]
}));

let ComicPane = Container.template($ => ({
  name: 'comicPane',
  left: 0, right: 0, top: 0, bottom: 0, skin: silverSkin,
  contents: [

  ]
}));

let LoadButton = Container.template($ => ({
  left: 0, right: 0, top: 0, bottom: 0, skin: blueSkin,
  active: true,
  contents: [
    new Label({
      left: 0, right:0, top: 0, bottom: 0,
      style: buttonStyle,
      string: 'Load Comic'
    })
  ],
  behavior: Behavior({
    onTouchEnded(container, id, x, y, ticks) {
      getImg(function(comicUrl) {
        let comicImg = new Picture({height: 200, url: comicUrl});
        container.container[1].add(comicImg);
      });
    }
  })
}));

function getImg(uiCallback) {
    var url = 'http://xkcd.com/info.0.json';
    var b;
    var smallText = new Style({ font: "24px", color: "black"});
    var smallerText = new Style({ font: "18px", color: "black"});
    var StringTemplate = Label.template($ => ({    	left: 0, right: 0, top: -50, bottom: 0,    	style: $.style,    	string: $.string	}));
	let blankc = new Content({ 
            left: 0, right: 0, top: 0, bottom: 0,     		skin: whiteSkin, active: true
    });
    application.add(blankc);
    let message = new Message(url);
    let sl = new Container({skin: silverSkin, width: 120, height: 20, top: 50, left: 0, active: true, 
          behavior: Behavior({ onTouchEnded: function(content) {
          	let promise2 = message.invoke(Message.JSON).then(text => {
          		url = "http://xkcd.com/" + (text.num - 1) + "/info.0.json"
          	});
          	message = new Message(url);
          	let promise3 = message.invoke(Message.JSON).then(text => {
      			if (0 == message.error && 200 == message.status) {
          			try {
           			 let blankc = new Content({ 
            			left: 0, right: 0, top: 70, bottom: 0,     					skin: whiteSkin, active: true
    					});
    			application.add(blankc);
            	let s = new StringTemplate({style: smallText, top: -60, string: text.safe_title});
            	let p = new Picture({width: 320, height: 140, top: 105, url: text.img});
            	application.add(s);
            	application.add(p);
            
          		}
          			catch (e) {
            		throw('Web service responded with invalid JSON!\n');
          			}
      			}
      			else {
          			trace('Request Failed - Raw Response Body: *'+text+'*'+'\n');
      			}
   			 });
            
            		
          	}}),
          contents: 
          [ new Label({style: smallerText, font: "12px", top: 0, left: 0, string: "Previous Comic"})
          ]
     });
    application.add(sl)
    let sr = new Container({skin: silverSkin, width: 120, height: 20, top: 50, right: 0, active: true, 
          behavior: Behavior({ onTouchEnded: function(content) {
          	let promise2 = message.invoke(Message.JSON).then(text => {
          		url = "http://xkcd.com/" + (text.num + 1) + "/info.0.json"
          	});
          	message = new Message(url);
          	let promise3 = message.invoke(Message.JSON).then(text => {
      			if (0 == message.error && 200 == message.status) {
          			try {
           			 let blankc = new Content({ 
            			left: 0, right: 0, top: 70, bottom: 0,     					skin: whiteSkin, active: true
    					});
    			application.add(blankc);
            	let s = new StringTemplate({style: smallText, top: -60, string: text.safe_title});
            	let p = new Picture({width: 320, height: 140, top: 105, url: text.img});
            	application.add(s);
            	application.add(p);
            
          		}
          			catch (e) {
            		throw('Web service responded with invalid JSON!\n');
          			}
      			}
      			else {
          			trace('Request Failed - Raw Response Body: *'+text+'*'+'\n');
      			}
   			 });
            
            		
          	}}),
    	
    	contents: 
            	[ new Label({style: smallerText, font: "12px", top: 0, left: 0, string: "     Next Comic"})
            	]
     });
    application.add(sr);
    let sm = new Container({skin: silverSkin, width: 120, height: 20, top: 0, right: 100, active: true, 
          behavior: Behavior({ onTouchEnded: function(content) {
            let message2 = new Message("");
            let message3 = new Message("");
          	let promise2 = message.invoke(Message.JSON).then(text => {
          		let title = text.safe_title;
          		title = title.replace(/\s/g, '+');
          		message2 = new Message("https://api.flickr.com/services/rest/?method=flickr.photos.search&api_key=e904f6fd166f683b7e41a95949d9fc42&text=\"" + title + "\"&format=json&nojsoncallback=1");
          		let promise3 = message2.invoke(Message.JSON).then(json => {
      			
           			 let blankc = new Content({ 
            			left: 0, right: 0, top: 105, bottom: 0,     					skin: whiteSkin, active: true
    					});
    			application.add(blankc);
    			let id = json.photos.photo[0].id;
    			trace(id + "\n");
    			message3 = new Message("https://api.flickr.com/services/rest/?method=flickr.photos.getSizes&api_key=e904f6fd166f683b7e41a95949d9fc42&photo_id=" + id + "&format=json&nojsoncallback=1");
            	let promise4 = message3.invoke(Message.JSON).then(text => {
                	let p = new Picture({width: 320, height: 140, top: 105, url: text.sizes.size[4].source});
            		application.add(p);
             	});
            
          		
   			 });
          	});
          	
   			 
   			 	
          	
            
            		
          	}}),
    contents: 
            	[ new Label({style: smallerText, font: "12px", top: 0, left: 0, string: "  Related Image"})
            	]
    });
    application.add(sm);
    let promise = message.invoke(Message.JSON).then(text => {
      if (0 == message.error && 200 == message.status) {
          try {
           
            let s = new StringTemplate({style: smallText, top: -60, string: text.safe_title});
            let p = new Picture({width: 320, height: 150, top: 105, url: text.img});
            application.add(s);
            application.add(p);
            
          }
          catch (e) {
            throw('Web service responded with invalid JSON!\n');
          }
      }
      else {
          trace('Request Failed - Raw Response Body: *'+text+'*'+'\n');
      }
    });
}

let mainContainer = new MainContainer();
application.add(mainContainer);