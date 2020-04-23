var DO_AUDIO = true;
var music;
var buttontick;
var endmessage;

var NEW_HIGHSCORE = false;

function setCookie(name, value)
{
	document.cookie = name + "=" + escape(value) + "; path=/";
}

function getCookie(name)
{
	var dc = document.cookie;
	var cname = name + "=";

	if (dc.length > 0)
	{
		begin = dc.indexOf(cname);

		if (begin != -1)
		{
			begin += cname.length;
			end = dc.indexOf(";", begin);

			if (end == -1) end = dc.length;
				return unescape(dc.substring(begin, end));
		}
	}

	return 0;
}

class MainMenu extends Phaser.Scene
{
	constructor()
	{
		super("MainMenu");
	}
	init(data)
	{
		NEW_HIGHSCORE = false;

		if (data.score)
		{
			if (getCookie("highscore") > data.score)
				endmessage = "You retreated! Candy collected: " + data.score
			else
			{
				endmessage = "You retreated! Candy collected: " + data.score
				NEW_HIGHSCORE = true
				setCookie("highscore", data.score);
			}
		}
		else if (data.fromCandyHunt)
			endmessage = "You were killed and taken to Area 51"
		else
			endmessage = false
	}
	preload()
	{
		console.log("Loading MainMenu Assets");
		this.load.image("mainmenu_background", "assets/background.png");
		this.load.image("mainmenu_star", "assets/star.png");

		this.load.image("mainmenu_button_up", "assets/button_grey_up.png");
		this.load.image("mainmenu_button_down", "assets/button_grey_down.png");

		if (this.sys.game.device.audio.mp3)
			this.load.audio("mainmenu_music", "assets/music/main_menu.mp3");
		else
		{
			console.log("Not loading audio. Device doesn't support any of our file types");
			DO_AUDIO = false;
		}
	}
	create()
	{
		let {width, height} = this.sys.game.canvas;

		if (DO_AUDIO)
		{
			music = this.sound.add("mainmenu_music", {loop: true});

			music.play();
		}

		// Add stars
		for (let a = 0; a < 100+random(100); ++a)
			this.add.image(random(width), random(height), "mainmenu_star").setScale(0.2+(random(2)*0.1)).setRotation(random(3));

		this.add.text(width/2, 16, "Trick Or Retreat", {fontFamily: "xolonium", fontSize: height/10, color: "#ff8500"}).setOrigin(0.5, 0).setShadow(2, 2, "#ff5b00", 1);

		var playbutton = this.add.sprite(width/2, height/2.2, "mainmenu_button_up").setScale(width/175, height/130).setOrigin(0.5, 0.5).setInteractive();
		var text = this.add.text(width/2, height/2.2, "Play", {fontFamily: "xolonium", fontSize: height/18.5, color: "#ff8500"}).setOrigin(0.5, 0.6);

		this.add.text(width/2, 64 + height/10, (NEW_HIGHSCORE ? "New highscore: " : "Highscore: ") + getCookie("highscore"), {
			fontFamily: "xolonium",
			fontSize: height/15,
			color: "#ff5b00",
			wordWrap: {width: width-64, useAdvancedWrap: true}
		}).setOrigin(0.5, 1).setShadow(1, 1, "#ff5b00", 1);

		if (endmessage)
			this.add.text(width/2, 96 + height/10, endmessage, {
				fontFamily: "xolonium",
				fontSize: height/18.5,
				color: "#00f603",
				wordWrap: {width: width-64, useAdvancedWrap: true}
			}).setOrigin(0.5, 0).setShadow(1, 1, "#00da03", 1);

		playbutton.on(
			"pointerover",
			function on_buttonover()
			{
				this.button.setTexture("mainmenu_button_down");
				this.text.setColor("#ff5b00");
				this.text.setOrigin(0.5, 0.5);
			},
			{button: playbutton, text: text}
		)

		playbutton.on(
			"pointerout",
			function on_buttonover()
			{
				this.button.setTexture("mainmenu_button_up");
				this.text.setColor("#ff8500");
				this.text.setOrigin(0.5, 0.6);
			},
			{button: playbutton, text: text}
		)

		playbutton.on(
			"pointerdown",
			function on_buttonover()
			{
				music.stop();
				this.scene.start("CandyHunt");
			},
			this
		)
	}
}

function random(max)
{
	return Math.floor(Math.random() * Math.floor(max));
}
