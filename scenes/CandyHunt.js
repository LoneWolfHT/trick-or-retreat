var DO_AUDIO = true;
var GAME_OVER = false;
var STARTED_ENDGAME = false;

var music, pickup_sound, explosion;
var player, helicopter, ufo;
var heli_rounds, heli_explosions;

var drop_timer;
var hdir;
var hstats = {
	velocity: 100,
	fire_rate: 2.0 * 1000,
	rate_increase: 0.1 * 1000,
	fire_limit: 0.4 * 1000,
}

const HELI_ROUND_SIZE = 16;

const BUILDING_RAD = 160; // Building width / 2
const PLAYER_WIDTH = 32; // Width of player sprite
const DOORSTEP_WIDTH = 32;

var DOORSTEPS = [];
var SCORE = 0;
var SCORE_TEXT;

const MUSIC_UPSTAGE_INTERVAL = 6;
var SCENE_MUSIC_COUNT = 0;
var MUSIC_STAGE = 1;
var PLAYING_MUSIC = false;
var VOLUME = 0.6;

function reset_values()
{
	DO_AUDIO = true;

	for (const obj of [music, pickup_sound, explosion, player, ufo, helicopter, heli_rounds, heli_explosions, drop_timer])
	if (obj)
	obj.destroy();

	music = pickup_sound = explosion = null;
	player = helicopter = null;
	heli_rounds = heli_explosions = null;

	drop_timer = null;
	hdir = null;
	hstats = {
		velocity: 100,
		fire_rate: 2.0 * 1000,
		rate_increase: 0.1 * 1000,
		fire_limit: 0.4 * 1000,
	}

	DOORSTEPS = [];
	SCORE = 0;
	SCORE_TEXT = null;

	SCENE_MUSIC_COUNT = 0;
	MUSIC_STAGE = 1;
	PLAYING_MUSIC = false;

	GAME_OVER = false;
	STARTED_ENDGAME = false;
}

class CandyHunt extends Phaser.Scene
{
	constructor()
	{
		super("CandyHunt");
	}
	preload()
	{
		console.log("Loading CandyHunt Assets");

		this.load.image("sidewalk", "assets/sidewalk.png");

		this.load.image("building", "assets/building.png");
		this.load.image("door", "assets/door.png")
		this.load.image("doorstep_unlooted", "assets/doorstep.png");
		this.load.image("doorstep_looted", "assets/doorstep_looted.png");

		this.load.image("trashcan", "assets/trashcan.png");

		this.load.spritesheet("alien", "assets/alien.png", {frameWidth: 32, frameHeight: 32});
		this.load.spritesheet("ufo", "assets/ufo.png", {frameWidth: 32, frameHeight: 32});
		this.load.spritesheet("helicopter", "assets/helicopter.png", {frameWidth: 64, frameHeight: 32});

		this.load.image("bullet", "assets/bullet.png");
		this.load.image("explosion", "assets/boom.png");

		if (this.sys.game.device.audio.mp3)
		{
			this.load.audio("candyhunt_1", "assets/music/candyhunt_1.mp3");
			this.load.audio("candyhunt_2", "assets/music/candyhunt_2.mp3");
			this.load.audio("candyhunt_3", "assets/music/candyhunt_3.mp3");
			this.load.audio("pickup_candy", "assets/sounds/pickup.mp3");
			this.load.audio("explode", "assets/sounds/explode.mp3");

			if (MUSIC_STAGE == 1)
				VOLUME = 0.6;
			else if (MUSIC_STAGE == 2)
				VOLUME = 0.8;
			else if (MUSIC_STAGE == 3)
				VOLUME = 1.0;
		}
		else
		{
			console.log("Not loading audio. Device doesn't support any of our file types");
			DO_AUDIO = false;
		}
	}
	create()
	{
		let {width, height} = this.sys.game.canvas;

		if (DO_AUDIO && !PLAYING_MUSIC)
		{
			music = this.sound.add("candyhunt_" + MUSIC_STAGE, {loop: true, volume: VOLUME});
			pickup_sound = this.sound.add("pickup_candy");
			explosion = this.sound.add("explode");

			music.play();
			PLAYING_MUSIC = true;
		}

		// Add stars
		for (let a = 0; a < Phaser.Math.Between(100, 200); ++a)
			this.add.image(random(width), random(height), "mainmenu_star").setScale(0.2+(random(2)*0.1)).setRotation(random(3));

		// Place Sidewalk
		var sidewalk = this.physics.add.staticGroup();

		sidewalk.create(0, height-16, "sidewalk").setOrigin(0, 0).setScale(width/16, 1).refreshBody();

		// Place buildings

		for (let i = 0; i < 4; ++i)
		{
			let pos = Phaser.Math.Between(BUILDING_RAD + ((width * i) / 4), ((width * (i+1)) / 4) - BUILDING_RAD);

			this.add.image(pos, height-16, "building").setOrigin(0.5, 1);
			this.add.image(pos, height-16, "door").setOrigin(0.5, 1);
			DOORSTEPS.push({pos: pos, img: this.add.image(pos, height-16, "doorstep_unlooted").setOrigin(0.5, 1)});
		}

		// Place trash cans
		var trashcans = this.physics.add.staticGroup();

		for (let count = 1; count < random(5);)
		{
			let pos = random(width - (PLAYER_WIDTH*2)) + PLAYER_WIDTH;
			let touches_door = false;

			for (const door of DOORSTEPS)
				if (Math.abs(door.pos - pos) <= PLAYER_WIDTH*2.5)
				{
					touches_door = true;
					break;
				}

			if (!touches_door)
			{
				trashcans.create(pos, height-16, "trashcan").setOrigin(0.5, 1).refreshBody();
				++count;
			}
		}

		// Add score text
		SCORE_TEXT = this.add.text(0, 0, "CANDY: " + SCORE, {fontFamily: "xolonium", fontSize: height/25, color: "#ff8500"}).setOrigin(0, 0);

		// Add helicopter
		helicopter = this.physics.add.sprite(width/2+(random(100)-50), height/2, "helicopter").setOrigin(0.5, 1).setScale(2, 2).setMaxVelocity(250, 0);

		this.anims.create({
			key: "heli_flying",
			frames: this.anims.generateFrameNumbers("helicopter", {start: 0, end: 1}),
			frameRate: 20,
			repeat: -1
		});

		helicopter.anims.play("heli_flying");

		// Add UFO
		ufo = this.physics.add.sprite(-16, -32, "ufo").setOrigin(0.5, 1).setScale(2, 2);

		ufo.body.setEnable(false);
		ufo.body.setGravity(0, 0);
		ufo.body.setDrag(0, 0);

		this.physics.add.collider(ufo, sidewalk);

		this.anims.create({
			key: "flying",
			frames: [{key: "ufo", frame: 0}],
		});

		this.anims.create({
			key: "flying_nearground",
			frames: [{key: "ufo", frame: 1}],
		});

		this.anims.create({
			key: "beaming",
			frames: [{key: "ufo", frame: 2}],
		});

		ufo.anims.play("flying");
		ufo.setDataEnabled();

		// Add player
		player = this.physics.add.sprite(16, height-16, "alien").setOrigin(0.5, 1);

		player.body.setGravityY(500);
		player.setCollideWorldBounds(true);

		this.physics.add.collider(player, sidewalk);
		this.physics.add.collider(player, trashcans);

		this.anims.create({
			key: "facing",
			frames: [{key: "alien", frame: 0}],
		});

		this.anims.create({
			key: "facing_withcandy",
			frames: [{key: "alien", frame: 1}],
		});

		this.anims.create({
			key: "left",
			frames: [{key: "alien", frame: 2}],
		});

		this.anims.create({
			key: "dead",
			frames: [{key: "alien", frame: 3}],
		});

		// Set up helicopter rounds and firing
		heli_rounds = this.physics.add.group()
		heli_explosions = this.physics.add.group()

		this.physics.add.collider(sidewalk, heli_rounds, on_round_hit_sidewalk, null, this);
		this.physics.add.collider(player, heli_rounds, on_round_hit_player, null, this);

		drop_timer = this.time.addEvent({loop: true, startAt: 0, delay: hstats.fire_rate, callback: drop_round})
	}
	update(time, delta)
	{
		let {width, height} = this.sys.game.canvas;

		if (!player || !player.body)
			return;

		if (!GAME_OVER)
		{
			let a_key = this.input.keyboard.addKey("A");
			let d_key = this.input.keyboard.addKey("D");
			let w_key = this.input.keyboard.addKey("W");
			let e_key = this.input.keyboard.addKey("E");
			let space_key = this.input.keyboard.addKey("SPACE");

			if (a_key.isDown)
			{
				player.setVelocityX(-300);
				player.anims.play("left");
				player.setFlipX(false);
			}
			else if (d_key.isDown)
			{
				player.setVelocityX(300);
				player.anims.play("left");
				player.setFlipX(true);
			}
			else if (w_key.isDown)
			{
				for (const door of DOORSTEPS)
					if (Math.abs(door.pos - player.x) <= DOORSTEP_WIDTH)
					{
						++SCORE;
						SCORE_TEXT.setText("CANDY: " + SCORE);

						door.img.setTexture("doorstep_looted");

						DOORSTEPS.splice(DOORSTEPS.indexOf(door), 1);

						if (DO_AUDIO)
							pickup_sound.play();
					}

				player.setVelocityX(0);
				player.anims.play("facing");
			}
			else
			{
				player.setVelocityX(0);
				player.anims.play("facing");
			}

			if (space_key.isDown && player.body.touching.down)
				player.body.setVelocityY(-250);

			if (player.x >= this.sys.game.canvas.width-PLAYER_WIDTH)
			{
				if (SCENE_MUSIC_COUNT < MUSIC_UPSTAGE_INTERVAL)
					++SCENE_MUSIC_COUNT;
				else if (MUSIC_STAGE < 3)
				{
					this.sound.removeByKey("candyhunt_" + MUSIC_STAGE);
					PLAYING_MUSIC = false;

					SCENE_MUSIC_COUNT = 0;
					++MUSIC_STAGE;
				}

				hstats.velocity += 10;

				if (hstats.fire_rate - hstats.rate_increase >= hstats.fire_limit)
				{
					hstats.fire_rate -= hstats.rate_increase;
					drop_timer.timeScale = hstats.fire_rate;
				}

				this.scene.restart("CandyHunt");
			}

			if (e_key.isDown && !ufo.data.values.RESCUE_PLAYER)
			{
				GAME_OVER = true;
				STARTED_ENDGAME = true;
				ufo.body.setEnable(true);
				ufo.data.set("RESCUE_PLAYER", true);
			}
		}
		else if (!STARTED_ENDGAME)
		{
			STARTED_ENDGAME = true;
			this.time.delayedCall(3 * 1000, function() {
				this.scene.start("MainMenu", {fromCandyHunt: true});
				reset_values();
			}, null, this)
		}

		// Make helicopter track player

		if (Math.abs(player.x - helicopter.x) >= PLAYER_WIDTH)
		{
			let dir = Math.sign(player.x - helicopter.x)

			switch (dir)
			{
				case -1:
					helicopter.setVelocityX(-hstats.velocity);
					helicopter.setFlipX(false);
					hdir = -1
					break;
				case 1:
					helicopter.setVelocityX(hstats.velocity);
					helicopter.setFlipX(true);
					hdir = 1
					break;
			}
		}

		if (ufo.data.get("RESCUE_PLAYER"))
		{
			if (!ufo.data.values.BEAMING)
				if (ufo.y > height/3)
					ufo.anims.play("flying_nearground");
				else
					ufo.anims.play("flying")

			if (!ufo.data.values.HAS_PLAYER && !ufo.data.values.LEAVING)
			{
				if (Math.abs(ufo.x - player.x) > 16) // Fly over the top of the player
					ufo.setVelocityX(Math.sign(player.x - ufo.x) * 500);
				else if (Math.abs(ufo.y - player.y) > 16) // Fly down to the player
				{
					ufo.setVelocityX(0);
					ufo.setVelocityY(300);
				}
				else if (!ufo.data.values.BEAMING) // Beam player up
				{
					ufo.data.set("BEAMING", true)
					ufo.anims.play("beaming");

					this.time.delayedCall(1 * 1000, function() {
						player.setVisible(false);
						ufo.data.set("HAS_PLAYER", true);
						ufo.data.set("LEAVING", true);
						ufo.data.set("BEAMING", false)
						ufo.setVelocityY(-700);
					}, null, this)
				}
			}
			else if (ufo.y <= 0)
			{
				this.scene.start("MainMenu", {score: SCORE, fromCandyHunt: true});
				reset_values();
			}
		}
	}
}

function drop_round()
{
	if (GAME_OVER)
		return;

	heli_rounds.create(helicopter.x + (hdir * 16), helicopter.y, "bullet").setVelocityY(350);
}

function on_round_hit_sidewalk(sidewalk, bullet)
{
	if (GAME_OVER)
		return;

	var temp = heli_explosions.create(bullet.x, bullet.y, "explosion").setMaxVelocity(0, 0).setScale(2, 2).setImmovable();
	this.time.delayedCall(1 * 1000, function() {temp.destroy();});

	bullet.destroy();

	if (DO_AUDIO)
		explosion.play();
}

function on_round_hit_player(player, bullet)
{
	if (GAME_OVER)
		return;

	bullet.destroy();
	drop_timer.destroy();

	if (DO_AUDIO)
		explosion.play();

	GAME_OVER = true;
	player.setVelocity(0, 0);
	player.setImmovable();
	player.anims.play("dead");
}
