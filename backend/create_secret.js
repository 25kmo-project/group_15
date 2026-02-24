//Import Node.js crypto module for cryptographic functions
const crypto = require('crypto');
//Generate 64 random bytes and convert to a Base64 string
//This can be used as a secret key for JWT or other encryption purposes
console.log(crypto.randomBytes(64).toString('base64'));