
function deleteFile() {
	const fileName = document.getElementById('deleteFileName').value;
	if (fileName) {
		fetch(`/uploads/${fileName}`, {
			method: 'DELETE'
		})
		.then(response => {
			if (response.ok) {
				console.log('File deleted successfully.');
			} else {
				console.log('File deletion failed.');
			}
		})
		.catch(error => {
			console.error('Error:', error);
			console.log('File deletion failed.');
		});
	} else {
		console.log('Please enter a file name.');
	}
}
