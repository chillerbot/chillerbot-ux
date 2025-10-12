

# Client Icons

# Kaizo Network
kz_image_kaizoicon= Image("kz_kaizoicon", "customclients/kaizoicon.png")
container.images.Add(kz_image_kaizoicon)
set_kz_kaizoicon = SpriteSet("kz_kaizoicon", kz_image_kaizoicon, 1, 1)
container.spritesets.Add(set_kz_kaizoicon)
container.sprites.Add(Sprite("kz_kaizoicon", set_kz_kaizoicon, 0, 0, 1, 1))