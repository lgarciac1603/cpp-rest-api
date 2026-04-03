# API Usage Guide (Example Requests)

This document gives you a quick reference for how to use the `cpp-rest-api` endpoints with `curl`. Use these examples against `http://localhost:8080` after you start the service (`docker-compose up --build`).

## 1. Create a new user

```bash
curl -X POST http://localhost:8080/users \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "username=john&email=john@example.com&password=secret123"
```

Expected response:

- Success: `201 Created` with `{"message":"User created"}`.
- Username exists: `409 Conflict` with `{"error":"username_already_exists"}`.
- Email exists: `409 Conflict` with `{"error":"email_already_exists"}`.

## 2. List all users

```bash
curl -X GET http://localhost:8080/users
```

Expected response: JSON array of users.

## 3. Get user by ID

```bash
curl -X GET http://localhost:8080/users/1
```

Expected response: JSON object of the requested user.

## 4. Update user email

```bash
curl -X PUT http://localhost:8080/users/1 \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "email=john.updated@example.com"
```

Expected response: JSON object of the updated user.

## 5. Delete user

```bash
curl -X DELETE http://localhost:8080/users/1
```

Expected response: empty body or status message.

## Authentication and session management

### 6. Login (create session)

```bash
curl -X POST http://localhost:8080/sessions \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "email=john@example.com&password=secret123"
```

Expected response: JSON with `access_token` and `refresh_token`.

### 7. Get current user `me` (requires bearer token)

```bash
curl -X GET http://localhost:8080/me \
  -H "Authorization: Bearer <access_token>"
```

Replace `<access_token>` with the token from login.

### 8. Refresh access token

```bash
curl -X POST http://localhost:8080/sessions/refresh \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "refresh_token=<refresh_token>"
```

Expected response: JSON with a new `access_token` and `expires_in`.

- If token is invalid or expired: `401 Unauthorized` with `{"error":"invalid_refresh_token"}`.

### 9. Logout (invalidate refresh token)

```bash
curl -X DELETE http://localhost:8080/sessions \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "refresh_token=<refresh_token>"
```

Expected response:

- Success: `204 No Content` (token invalidated).
- Token invalid/already logged out: `401 Unauthorized` with `{"error":"invalid_refresh_token"}`.

## Notes

- For all non-GET calls with a body, use `Content-Type: application/x-www-form-urlencoded`.
- For multiple test users, create users and keep track of user IDs for `GET/PUT/DELETE`.
- Ensure PostgreSQL is running and the API service is started before sending requests.
