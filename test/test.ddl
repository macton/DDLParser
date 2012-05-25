select Weapon, author( "Andre de Leiradella" ), description( "Weapons available to the player" ), label( "Weapon" )
{
  fist,           description( "Bare hands" ),               label( "Fist" );
  chainsaw,       description( "A la Chainsaw Massacre" ),   label( "Chainsaw" );
  pistol,         description( "Simple pistol" ),            label( "Pistol" ), default;
  shotgun,        description( "A single-barrel shotgun" ),  label( "Shotgun" );
  chaingun,       description( "A machine gun" ),            label( "Chaingun" );
  rocketLauncher, description( "Portable rocket launcher" ), label( "Rocket launcher" );
  plasmaGun,      description( "Plasma gun" ),               label( "Plasma gun" );
  BFG9000,        description( "*The* BFG 9000" ),           label( "BFG 9000" );
}

bitfield Powerup, author( "Andre de Leiradella" ), description( "Powerup pickups" ), label( "Powerup" )
{
  none,                description( "Help me!" ), empty;
  radiationSuit,       description( "Makes the player immune to radiation for a limited time" ),             label( "Radiation suit" );
  partialInvisibility, description( "Makes the player almost invisible to enemies for a limited time" ),     label( "Partial invisibility" );
  invulnerability,     description( "Makes the player invulnerable for a limited time" ),                    label( "Invulnerability" );
  computerMap,         description( "Gives the complete map of the level to the player" ),                   label( "Computer map" );
  lightVisor,          description( "Allows the player to see in the dark for a limited time" ),             label( "Light visor" );
  berserk,             description( "Gives the player the ability to quickly kill enemies with his fists" ), label( "Berserk" );
  all,                 value( radiationSuit | partialInvisibility | invulnerability | computerMap | lightVisor | berserk );
}

struct Position
{
  f32 x,     value( 0 );
  f32 y,     value( 0 );
  f32 angle, value( 0 ), description( "The direction the player is looking at ( degrees )" );
}

struct Mariner, author( "Andre de Leiradella" ), description( "The player character" ), label( "Player" )
{
  u32        health,     value( 100 ),                                 description( "The player's health" );
  Weapon     weapon,     value( pistol ),                              description( "The player's current weapon" );
  Powerup    powerup,    value( berserk ),                             description( "The player's powerups" );
  i32[ 8 ]      ammunition, value( { 0, 0, 20, -1, -1, -1, -1, -1 } ), description( "The ammunition of each weapon, -1 means the player doesn't have it" );
  string     name,       value( "Mariner" ),                           description( "The player's name for multiplayer sessions" );
  Position   position,   value( { x = 100, y = 120 } ),                description( "The player's position" );
  Position[] deaths,                                                   description( "Places the player has died in" );
  u32{ string } completed,                                             description( "Percent completed for each level indexed by level name" );
}
