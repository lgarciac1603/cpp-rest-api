BEGIN;

INSERT INTO users (username, email, password_hash) VALUES
('alice',   'alice@test.com',   'hash_alice'),
('bob',     'bob@test.com',     'hash_bob'),
('carol',   'carol@test.com',   'hash_carol'),
('dave',    'dave@test.com',    'hash_dave'),
('eve',     'eve@test.com',     'hash_eve'),
('frank',   'frank@test.com',   'hash_frank'),
('grace',   'grace@test.com',   'hash_grace'),
('heidi',   'heidi@test.com',   'hash_heidi'),
('ivan',    'ivan@test.com',    'hash_ivan'),
('judy',    'judy@test.com',    'hash_judy');

COMMIT;
