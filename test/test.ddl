select Weapon, author( "Andre de Leiradella" ), description( "Weapons available to the player" ), label( "Weapon" )
{
  kFist,           description( "Bare hands" ),               label( "Fist" );
  kChainsaw,       description( "A la Chainsaw Massacre" ),   label( "Chainsaw" );
  kPistol,         description( "Simple pistol" ),            label( "Pistol" ), default;
  kShotgun,        description( "A single-barrel shotgun" ),  label( "Shotgun" );
  kChaingun,       description( "A machine gun" ),            label( "Chaingun" );
  kRocketLauncher, description( "Portable rocket launcher" ), label( "Rocket launcher" );
  kPlasmaGun,      description( "Plasma gun" ),               label( "Plasma gun" );
  kBFG9000,        description( "*The* BFG 9000" ),           label( "BFG 9000" );
}

bitfield Powerup, author( "Andre de Leiradella" ), description( "Powerup pickups" ), label( "Powerup" )
{
  kNone,                description( "Help me!" ), empty;
  kRadiationSuit,       description( "Makes the player immune to radiation for a limited time" ),             label( "Radiation suit" );
  kPartialInvisibility, description( "Makes the player almost invisible to enemies for a limited time" ),     label( "Partial invisibility" );
  kInvulnerability,     description( "Makes the player invulnerable for a limited time" ),                    label( "Invulnerability" );
  kComputerMap,         description( "Gives the complete map of the level to the player" ),                   label( "Computer map" );
  kLightVisor,          description( "Allows the player to see in the dark for a limited time" ),             label( "Light visor" );
  kBerserk,             description( "Gives the player the ability to quickly kill enemies with his fists" ), label( "Berserk" );
  kAll,                 value( kRadiationSuit | kPartialInvisibility | kInvulnerability | kComputerMap | kLightVisor | kBerserk );
}

struct Position
{
  f32 x,     value( 0 );
  f32 y,     value( 0 );
  f32 angle, value( 0 ), description( "The direction the player is looking at ( degrees )" );
}

struct Mariner, author( "Andre de Leiradella" ), description( "The player character" ), label( "Player" )
{
  u32        health,   value( 100 ),                  description( "The player's health" );
  Weapon     weapon,   value( kPistol ),              description( "The player's current weapon" );
  Powerup    powerup,  value( kBerserk ),             description( "The player's powerups" );
  string     name,     value( "Mariner" ),            description( "The player's name for multiplayer sessions" );
  Position   position, value( { x = 100, y = 120 } ), description( "The player's position" );
}
