import os
from spotipy.oauth2 import SpotifyOAuth
import utils as utils
from spotipy.client import Spotify

scope = 'playlist-read-private'
auth_manager = SpotifyOAuth(scope=scope, cache_handler=utils.CacheFileHandler())
print("Client_ID:", auth_manager.client_id)
print("Cache handler:", auth_manager.cache_handler)
print("Open Browser:", auth_manager.open_browser)

print("Creating Spotify object")
sp = Spotify(auth_manager=auth_manager)
print("Done with Spotify")
print(sp.auth_manager.get_access_token(as_dict=False))


list_of_playlists = sp.current_user_playlists(limit=50)

print("Name of the Playlists:")
for i, result in enumerate(list_of_playlists["items"]):
    print(i+1, result["name"])
    
# Get playlist ID to fetch details
id = input("Playlist ID:")
playlist_details = sp.playlist(playlist_id=id)
    
for i, result in enumerate(playlist_details["tracks"]["items"]):
    print(i+1, result["track"]["name"], result['track']['album']['name'])
    
album_id = input("Album ID:")
results = sp.album(album_id=album_id)

for i, result in enumerate(results):
    print(i+1, result)
    
results = sp.me()
print(results)

artist_id = input("Artist ID:")
results = sp.artist(artist_id=artist_id)
print(results)
    
if os.path.exists(sp.auth_manager.cache_handler.cache_path):
    os.remove(sp.auth_manager.cache_handler.cache_path)
else:
    print("Cache does not exists.")
